---
path: "/ac-in-options"
posttype: "docs"
title: "ac_in (cont)"
---

```c
#include "ac_in.h"
```

All of the example code is found in examples/ac_in.  

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
