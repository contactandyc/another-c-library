#ifndef _ac_in_H
#define _ac_in_H

/* The ac_in object is meant for reading input (generally files, but file
   descriptors, sets of records, and blocks of memory can also be used).  The
   object can be configured as a single input or a set of inputs and then act
   as a single input.  This allows functions to be written that can operate
   on a variety of inputs (one file, many files, files and buffers mixed, lists
   of files, etc).  Sets of sorted files where each file is sorted can be
   joined together as a single unit and remain sorted.  If the sorted files
   are distinct internally, but not within the set, the join can be configured
   to reduce the non-distinct elements.

   Another useful feature supported by ac_in is built in compression.  Files
   with an extension of .gz or .lz4 are automatically read from their
   respective formats without the need to first decompress the files.

   In general, the ac_in object is meant to iterate over one or more files of
   records where the records are prefix formatted, delimiter formatted, or
   fixed length.  Additional formats can be added as needed, but these three
   cover most use cases.  The prefix format prefixes a 4 byte length at the
   beginning of each record.  The delimiter format expects a delimiter such as
   a newline.  The fixed length format expects all records to be a similar
   size.

   Because disks are slow and RAM is fast, buffering is built into the ac_in
   object.  A buffer size can be defined to reduce IO (particularly seeks).

   The ac_in object plays an important role in sorting files.  When large
   files are sorted, tmp files are written to disk which are themselves
   sorted.  To finally sort the data, the tmp files are joined and further
   reduced if necessary as an ac_in object.  The ac_out object allows one
   to call ac_out_in which can be used to avoid writing the final sort file
   if desired.  Chaining sorts benefits from this in that the tmp files can
   be piped directly into the next sort.

   Because the ac_in (and the ac_out) objects can be configured, it makes it
   possible to define pipelines of data with very little extra code.  The
   ac_schedule object heavily uses the ac_in and ac_out objects to allow for
   very large data transformations in a defined environment.
*/

#include "ac_in_base.h"

#include "ac_common.h"
#include "ac_io.h"
#include "ac_lz4.h"

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "ac-io/ac_in.h"

typedef ac_in_t *(*ac_in_init_cb)( void * );

typedef void (*ac_in_transform_cb)(ac_in_t *in, ac_out_t *out, void *arg);

/* ac_in_options_t is declared in ac-io/ac_in.h and not opaque.  h is expected
   to point to a structure of this type (and not NULL). */
void ac_in_options_init(ac_in_options_t *h);

/* Set the buffer size for reading input from files.  If the input is
   compressed, the buffer_size here is for the uncompressed content.
   Ideally, this would be large enough to support any record in the input.
   If an individual record is larger than the buffer_size, a temporary
   buffer will be created to hold the given record.  The temporary buffer
   should happen as the exception (if at all). */
void ac_in_options_buffer_size(ac_in_options_t *h, size_t buffer_size);

/* This should be called with one of the ac_io_format... methods.

   Prefix format (4 byte length prefix before each record),
   ac_in_options_format(&options, ac_io_prefix());

   Delimiter format (specify a character at the end of a record)
   ac_in_options_format(&options, ac_io_delimiter('\n'));

   Fixed format (all records are the same length)
   ac_in_options_format(&options, ac_io_fixed(<some_length>));

   Other formats may be added in the future such as compressed, protobuf, etc.
*/
void ac_in_options_format(ac_in_options_t *h, ac_io_format_t format);

/* This generally applies to opening compressed files which have a corrupt
   format.  If the format of the file is corrupt, abort() will be called
   instead of prematurely ending the file. */
void ac_in_options_abort_on_error(ac_in_options_t *h);

/* If a file has an incomplete record at the end, the record will be dropped
   unless it this is set. */
void ac_in_options_allow_partial_records(ac_in_options_t *h);

/* If a partial record exists at the end of a file, the record would normally
   be silently dropped (unless partial records are allowed above).  Setting
   this would cause the program to abort on a partial record. */
void ac_in_options_abort_on_partial(ac_in_options_t *h);

/* If a file is not found, abort (instead of treating it as an empty file). */
void ac_in_options_abort_on_file_not_found(ac_in_options_t *h);

/* If a file is empty, abort() */
void ac_in_options_abort_on_file_empty(ac_in_options_t *h);

/* This tag can be useful to distinguish one file from another.  It can also
   be used as follows...

   ac_in_options_tag(&options, <some_int>);
   ac_in_add(h, in, options.tag);
*/
void ac_in_options_tag(ac_in_options_t *h, int tag);

/* Indicate that the contents are compressed (if using a file descriptor or
   buffer).  filenames are determined to be compressed if they end in .gz
   or .lz4.  The buffer_size is the size to buffer compressed content which
   will default to buffer_size. */
void ac_in_options_gz(ac_in_options_t *h, size_t buffer_size);
void ac_in_options_lz4(ac_in_options_t *h, size_t buffer_size);
void ac_in_options_compressed_buffer_size(ac_in_options_t *h,
                                          size_t buffer_size);

/* Within a single cursor, reduce equal items.  In this case, it is assumed
   that the contents are sorted.  */
void ac_in_options_reducer(ac_in_options_t *h, ac_io_compare_cb compare,
                           void *compare_arg, ac_io_reducer_cb reducer,
                           void *reducer_arg);

/* The filename dictates whether the file is normal, gzip compressed (.gz
   extension), or lz4 compressed (.lz4 extension).  If options is NULL, default
   options will be used. NULL will be returned if the file cannot be opened.
*/
ac_in_t *ac_in_init(const char *filename, ac_in_options_t *options);

/* This is just like ac_in_init, except that it sets the format and buffer_size
   using parameters instead of requiring the ac_in_options_t structure.  It is
   meant to be a helper function which reduces required code for this use case.
*/
ac_in_t *ac_in_quick_init(const char *filename, ac_io_format_t format, size_t buffer_size);

/* Uses the file descriptor to create the input stream.  options dictate the
   compression state of input.  can_close should normally be true meaning that
   when the ac_in object is destroyed, the file should be closed.
*/
ac_in_t *ac_in_init_with_fd(int fd, bool can_close, ac_in_options_t *options);

/* This creates a stream from a buffer. options dictate the compression state
   of input.  can_free should normally be true meaning that when the ac_in
   object is destroyed, the buffer should be freed. */
ac_in_t *ac_in_init_with_buffer(void *buf, size_t len, bool can_free,
                                ac_in_options_t *options);

/* Use this to create an ac_in_t which allows cursoring over an array of
   ac_io_record_t structures. */
ac_in_t *ac_in_records_init(ac_io_record_t *records, size_t num_records,
                            ac_in_options_t *options);

/* Use this function to create an ac_in_t which allows multiple input streams */
ac_in_t *ac_in_ext_init(ac_io_compare_cb compare, void *arg,
                        ac_in_options_t *options);

/* Create an input which will open sequentially until cb returns NULL */
ac_in_t *ac_in_init_from_cb(ac_in_init_cb cb, void *arg);

/* Create an input which will open one file at a time in files */
ac_in_t *ac_in_init_from_list(ac_io_file_info_t *files, size_t num_files,
                              ac_in_options_t *options);

/* Create an empty cursor - still must be destroyed and won't return anything */
ac_in_t *ac_in_empty();

/* Useful to limit the number of records for testing */
void ac_in_limit(ac_in_t *h, size_t limit);

/* After in is destroyed, destroy the given output, useful in transformations */
void ac_in_destroy_out(ac_in_t *in, ac_out_t *out,
                       void (*destroy_out)(ac_out_t *out));

/* Transform the data and return a new cursor */
ac_in_t *ac_in_transform(ac_in_t *in, ac_io_format_t format, size_t buffer_size,
                         ac_io_compare_cb compare, void *compare_arg,
                         ac_io_reducer_cb reducer, void *reducer_arg,
                         ac_in_transform_cb transform, void *arg);

/* When there are multiple input streams, this would keep only the first equal
   record across the streams. */
void ac_in_ext_keep_first(ac_in_t *h);

/* When there are multiple input streams, set the reducer */
void ac_in_ext_reducer(ac_in_t *h, ac_io_reducer_cb reducer, void *arg);

/* The tag can be options->tag from init of in if that makes sense.  Otherwise,
  this can be useful to distinguish different input sources. The first param
  h must be initialized with ac_in_init_compare. */
void ac_in_ext_add(ac_in_t *h, ac_in_t *in, int tag);

/* Count the records and close the cursor */
size_t ac_in_count(ac_in_t *h);

/* Advance to the next record and return it. */
ac_io_record_t *ac_in_advance(ac_in_t *h);

/* Get the current record (this will be NULL if advance hasn't been called or
 * ac_in_reset was called). */
ac_io_record_t *ac_in_current(ac_in_t *h);

/* Make the next call to advance return the same record as the current.  This is
  particularly helpful when advancing in a loop until a given record. */
void ac_in_reset(ac_in_t *h);

/* Return the next equal record across all of the streams.  num_r will be the
   number of streams containing the next record.  It is assumed that each
   stream will have exactly one equal record. If there is only one stream,
   num_r will always be 1 until the stream is finished. */
ac_io_record_t *ac_in_advance_unique(ac_in_t *h, size_t *num_r);

/* Given the comparison function and arg, get all records which are equal and
   return them as an array.  This will work with one or more input streams. */
ac_io_record_t *ac_in_advance_group(ac_in_t *h, size_t *num_r,
                                    bool *more_records, ac_io_compare_cb compare,
                                    void *arg);

/* Destroy the input stream (or set of input streams) */
void ac_in_destroy(ac_in_t *h);

/* These are experimental and don't need documented yet */

typedef void (*ac_in_out_cb)(ac_out_t *out, ac_io_record_t *r, void *arg);
typedef void (*ac_in_out2_cb)(ac_out_t *out, ac_out_t *out2, ac_io_record_t *r,
                             void *arg);
typedef void (*ac_in_out_group_cb)(ac_out_t *out, ac_io_record_t *r,
                                  size_t num_r, bool more_records, void *arg);
typedef void (*ac_in_out_group2_cb)(ac_out_t *out, ac_out_t *out2,
                                   ac_io_record_t *r, size_t num_r,
                                   bool more_records, void *arg);

/* These are very common transformations.  All of these take a single input and
   write to one or two outputs.  The custom functions allow for custom filtering
   and various other transformations.  The group functions will group records
   based upon the compare function.  */

/* Write all records from in to out */
void ac_in_out(ac_in_t *in, ac_out_t *out);

/* Call custom callback for every input record (with out, record, and arg). */
void ac_in_out_custom(ac_in_t *in, ac_out_t *out, ac_in_out_cb cb, void *arg);

/* Write all records from in to both out and out2 */
void ac_in_out2(ac_in_t *in, ac_out_t *out, ac_out_t *out2);

/* Call custom callback for every input record (with out, out2, record, and
   arg). */
void ac_in_out_custom2(ac_in_t *in, ac_out_t *out, ac_out_t *out2,
                       ac_in_out2_cb cb, void *arg);

void ac_in_out_group(ac_in_t *in, ac_out_t *out, ac_io_compare_cb compare,
                     void *compare_arg, ac_in_out_group_cb group, void *arg);

void ac_in_out_group2(ac_in_t *in, ac_out_t *out, ac_out_t *out2,
                      ac_io_compare_cb compare, void *compare_arg,
                      ac_in_out_group2_cb group, void *arg);

#ifdef __cplusplus
}
#endif

#endif
