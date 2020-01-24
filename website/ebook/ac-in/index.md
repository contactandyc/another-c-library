---
path: "/ac-in"
posttype: "docs"
title: "ac_in"
---

```c
#include "ac_in.h"
```

The ac_in object is meant for reading input (generally files, but file descriptors, sets of records, and blocks of memory can also be used).  The object can be configured as a single input or a set of inputs and then act as a single input.  This allows functions to be written that can operate on a variety of inputs (one file, many files, files and buffers mixed, lists of files, etc).  Sets of sorted files where each file is sorted can be joined together as a single unit and remain sorted.  If the sorted files are distinct internally, but not within the set, the join can be configured to reduce the non-distinct elements.

Another useful feature supported by ac_in is built in compression.  Files with an extension of .gz or .lz4 are automatically read from their respective formats without the need to first decompress the files.

In general, the ac_in object is meant to iterate over one or more files of records where the records are prefix formatted, delimiter formatted, or fixed length.  Additional formats can be added as needed, but these three cover most use cases.  The prefix format prefixes a 4 byte length at the beginning of each record.  The delimiter format expects a delimiter such as a newline.  The fixed length format expects all records to be a similar size.

Because disks are slow and RAM is fast, buffering is built into the ac_in object.  A buffer size can be defined to reduce IO (particularly seeks).

The ac_in object plays an important role in sorting files.  When large files are sorted, tmp files are written to disk which are themselves sorted.  To finally sort the data, the tmp files are joined and further reduced if necessary as an ac_in object.  The ac_out object allows one to call ac_out_in which can be used to avoid writing the final sort file if desired.  Chaining sorts benefits from this in that the tmp files can be piped directly into the next sort.

Because the ac_in (and the ac_out) objects can be configured, it makes it possible to define pipelines of data with very little extra code.  The ac_schedule object heavily uses the ac_in and ac_out objects to allow for very large data transformations in a defined environment.

All of the example code is found in examples/ac_in.  


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
