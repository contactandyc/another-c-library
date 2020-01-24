---
path: "/ac-in"
posttype: "docs"
title: "ac_in"
---

```c
#include "ac_in.h"
```

The ac\_in object is meant for reading input (generally files, but file descriptors, sets of records, and blocks of memory can also be used).  The object can be configured as a single input or a set of inputs and then act as a single input.  This allows functions to be written that can operate on a variety of inputs (one file, many files, files and buffers mixed, lists of files, etc).  Sets of sorted files where each file is sorted can be joined together as a single unit and remain sorted.  If the sorted files are distinct internally, but not within the set, the join can be configured to reduce the non-distinct elements.

Another useful feature supported by ac\_in is built in compression.  Files with an extension of .gz or .lz4 are automatically read from their respective formats without the need to first decompress the files.

In general, the ac\_in object is meant to iterate over one or more files of records where the records are prefix formatted, delimiter formatted, or fixed length.  Additional formats can be added as needed, but these three cover most use cases.  The prefix format prefixes a 4 byte length at the beginning of each record.  The delimiter format expects a delimiter such as a newline.  The fixed length format expects all records to be a similar size.

Because disks are slow and RAM is fast, buffering is built into the ac\_in object.  A buffer size can be defined to reduce IO (particularly seeks).

The ac\_in object plays an important role in sorting files.  When large files are sorted, tmp files are written to disk which are themselves sorted.  To finally sort the data, the tmp files are joined and further reduced if necessary as an ac\_in object.  The ac\_out object allows one to call ac\_out\_in which can be used to avoid writing the final sort file if desired.  Chaining sorts benefits from this in that the tmp files can be piped directly into the next sort.

Because the ac\_in (and the ac\_out) objects can be configured, it makes it possible to define pipelines of data with very little extra code.  The ac\_schedule object heavily uses the ac\_in and ac\_out objects to allow for very large data transformations in a defined environment.

All of the example code is found in examples/ac\_in.  


## ac\_in\_init

```c
ac_in_t *ac_in_init(const char *filename, ac_in_options_t *options);
```
ac\_in\_init creates an input stream based upon a filename and options.  The filename dictates whether the file is normal, gzip compressed (.gz extension), or lz4 compressed (.lz4 extension).  If options are NULL, defaults will be used. NULL will be returned if the file cannot be opened.

## ac\_in\_init\_with\_fd

```c
ac_in_t *ac_in_init_with_fd(int fd, bool can_close, ac_in_options_t *options);
```
ac\_in\_init\_with\_fd creates an input stream based upon a file descriptor.  If the input stream is compressed, that should be set through ac\_in\_options\_gz or ac\_in\_options\_lz4.  Otherwise, if options are NULL, defaults will be used.  can_close should normally be true meaning that when the ac_in object is destroyed, the file should be closed.

## ac\_in\_init\_with\_buffer

```c
ac_in_t *ac_in_init_with_buffer(void *buf, size_t len, bool can_free,
                                ac_in_options_t *options);
```
ac\_in\_init\_with\_buffer creates a stream from a buffer. If the input stream is compressed, that should be set through ac\_in\_options\_gz or ac\_in\_options\_lz4.  Otherwise, if options are NULL, defaults will be used.  can_free should normally be true meaning that when the ac_in object is destroyed, the buffer should be freed.

## ac\_in\_records\_init

```c
ac_in_t *ac_in_records_init(ac_io_record_t *records, size_t num_records,
                            ac_in_options_t *options);
```
ac\_in\_records\_init creates a stream from an array of ac\_io\_record\_t structures.

## ac\_in\_ext\_init

```c
ac_in_t *ac_in_ext_init(ac_io_compare_f compare, void *arg,
                        ac_in_options_t *options);
```
ac\_in\_ext\_init creates an ac_in_t object which can be added to via ac\_in\_ext\_add to setup a multiple stream input.

## ac\_in\_init\_from\_list

```c
ac_in_t *ac_in_init_from_list(ac_io_file_info_t *files, size_t num_files,
                              ac_in_options_t *options);
```
ac\_in\_init\_from\_list create a stream which will open one file at a time for each of the files specified.

## ac\_in\_empty

```c
ac_in_t *ac_in_empty();
```
ac\_in\_empty creates an empty stream.  The stream must still be destroyed once it is no longer needed.

## ac\_in\_limit

```c
void ac_in_limit(ac_in_t *h, size_t limit);
```
ac\_in\_limit limits the number of records returned from the stream (usually used for testing).

## ac\_in\_destroy\_out

```c
void ac_in_destroy_out(ac_in_t *in, ac_out_t *out,
                       void (*destroy_out)(ac_out_t *out));
```
ac\_in\_destroy\_out destroys an output stream when the given input stream is destroyed.

## ac\_in\_ext\_keep\_first

```c
void ac_in_ext_keep_first(ac_in_t *h);
```
ac\_in\_ext\_keep\_first keeps only the first equal record across multiple input streams.

## ac\_in\_ext\_reducer

```c
void ac_in_ext_reducer(ac_in_t *h, ac_io_reducer_f reducer, void *arg);
```
ac\_in\_ext\_reducer reduces equal records across multiple input streams.

## ac\_in\_ext\_add

```c
void ac_in_ext_add(ac_in_t *h, ac_in_t *in, int tag);
```
ac\_in\_ext\_add The tag can be options->tag from init of in if that makes sense.  Otherwise, this can be useful to distinguish different input sources. The first param h must be initialized with ac_in_init_compare.

## ac\_in\_count

```c
size_t ac_in_count(ac_in_t *h);
```
ac\_in\_count counts the records and closes the cursor.

## ac\_in\_advance

```c
ac_io_record_t *ac_in_advance(ac_in_t *h);
```
ac\_in\_advance advances to the next record and returns it.

## ac\_in\_current

```c
ac_io_record_t *ac_in_current(ac_in_t *h);
```
ac\_in\_current returns the current record (this will be NULL if advance hasn't been called or
ac_in_reset was called).

## ac\_in\_reset

```c
void ac_in_reset(ac_in_t *h);
```
ac\_in\_reset makes the next call to advance return the same record as the current.  This is particularly helpful when advancing in a loop until a given record.

## ac\_in\_advance\_unique

```c
ac_io_record_t *ac_in_advance_unique(ac_in_t *h, size_t *num_r);
```
ac\_in\_advance\_unique returns the next equal record across all of the streams.  num_r will be the number of streams containing the next record.  It is assumed that each stream will have exactly one equal record. If there is only one stream, num_r will always be 1 until the stream is finished.

## ac\_in\_advance\_group

```c
ac_io_record_t *ac_in_advance_group(ac_in_t *h, size_t *num_r,
                                    bool *more_records, ac_io_compare_f compare,
                                    void *arg);
```
ac\_in\_advance\_group takes as an input a comparison function and an arg, get all records which are equal and return them as an array.  This will work with one or more input streams.  num\_r and more\_records are returned.  num\_r will be one or more unless the end of the input is reached.  If more\_records is true, then the next call to ac\_in\_advance\_group will have records which are equal to the current group.

## ac\_in\_destroy

```c
void ac_in_destroy(ac_in_t *h);
```
ac\_in\_destroy destroys the input stream (or set of input streams)


## ac\_in\_options\_init

```c
void ac_in_options_init(ac_in_options_t *h);
```
ac\_in\_options\_init initializes h to default values.  ac\_in\_options\_t is declared in impl/ac\_in.h and not opaque.  h is expected to point to a structure of this type (and not NULL).

## ac\_in\_options\_buffer\_size

```c
void ac_in_options_buffer_size(ac_in_options_t *h, size_t buffer_size);
```
ac\_in\_options\_buffer\_size sets the buffer size for reading input from files.  If the input is compressed, the buffer_size here is for the uncompressed content.  Ideally, this would be large enough to support any record in the input.  If an individual record is larger than the buffer_size, a temporary buffer will be created to hold the given record.  The temporary buffer should happen as the exception (if at all).

## ac\_in\_options\_format

```c
void ac_in_options_format(ac_in_options_t *h, ac_io_format_t format);
```
ac\_in\_options\_format sets the format of the records within the input.  This should be called with one of the ac\_io\_format... methods.

Prefix format (4 byte length prefix before each record)<br/>
```ac_in_options_format(&options, ac_io_prefix());```

Delimiter format (specify a character at the end of a record)<br/>
```ac_in_options_format(&options, ac_io_delimiter('\n'));```

Fixed format (all records are the same length)<br/>
```ac_in_options_format(&options, ac_io_fixed(<some_length>));```

Other formats may be added in the future such as compressed, protobuf, etc.

## ac\_in\_options\_abort\_on\_error

```c
void ac_in_options_abort_on_error(ac_in_options_t *h);
```
ac\_in\_options\_abort\_on\_error generally applies to opening compressed files which have a corrupt format.  If the format of the file is corrupt, abort() will be called instead of prematurely ending the file.

## ac\_in\_options\_allow\_partial\_records

```c
void ac_in_options_allow_partial_records(ac_in_options_t *h);
```
ac\_in\_options\_allow\_partial\_records will cause an incomplete record to be counted as a valid record at the end of the file.  By default records that are partial at the end of the file are dropped.

## ac\_in\_options\_abort\_on\_partial

```c
void ac_in_options_abort_on_partial(ac_in_options_t *h);
```

If a partial record exists at the end of a file, the record would normally be silently dropped (unless partial records are allowed above).  Setting this would cause the program to abort on a partial record.

## ac\_in\_options\_abort\_on\_file\_not\_found

```c
void ac_in_options_abort_on_file_not_found(ac_in_options_t *h);
```

If a file is not found, abort (instead of treating it as an empty file).

## ac\_in\_options\_abort\_on\_file\_empty

```c
void ac_in_options_abort_on_file_empty(ac_in_options_t *h);
```
If a file is empty, abort()

## ac\_in\_options\_tag

```c
void ac_in_options_tag(ac_in_options_t *h, int tag);
```
ac\_in\_options\_tag sets a tag on a file.  This tag can be useful to distinguish one file from another as all of the records from the given file with have the associated tag.

## ac\_in\_options\_gz
```c
void ac_in_options_gz(ac_in_options_t *h, size_t buffer_size);
```
ac\_in\_options\_gz indicates that the contents of the input are gzipped (if not using a filename).  Normally input is determined to be gzipped if the filename ends in .gz and sets a compressed buffer size (which defaults to the regular buffer size).


## ac\_in\_options\_lz4

```c
void ac_in_options_lz4(ac_in_options_t *h, size_t buffer_size);
```
ac\_in\_options\_lz4 indicates that the contents of the input are lz4 compressed (if not using a filename).  Normally input is determined to be lz4 compressed if the filename ends in .lz4 and sets a compressed buffer size (which defaults to the regular buffer size).

## ac\_in\_options\_compressed\_buffer\_size

```c
void ac_in_options_compressed_buffer_size(ac_in_options_t *h,
                                          size_t buffer_size);
```
ac\_in\_options\_compressed\_buffer\_size sets the compressed buffer size.  The buffer_size is the size to buffer compressed content which will default to buffer_size.

## ac\_in\_options\_reducer

```c
void ac_in_options_reducer(ac_in_options_t *h, ac_io_compare_f compare,
                           void *compare_arg, ac_io_reducer_f reducer,
                           void *reducer_arg);
```
ac\_in\_options\_reducer sets the compare and reducer on a given input.  Within a single cursor, reduce equal items.  In this case, it is assumed that the contents are sorted and that the compare method passed in will form groups of one or more records.
