---
path: "/ac-out"
posttype: "docs"
title: "ac_out"
---

```c
#include "ac_out.h"
```

```c
ac_out_t *ac_out_init(const char *filename, ac_out_options_t *options);
```
ac\_out\_init opens a file for writing with a given filename

```c
ac_out_t *ac_out_init_with_fd(int fd, bool fd_owner, ac_out_options_t *options);
```
ac\_out\_init\_with\_fd associates an open file descriptor to an ac\_out object.  If fd\_owner is true, the file descriptor will be closed when this is destroyed.

```c
ac_out_t *ac_out_ext_init(const char *filename, ac_out_options_t *options,
                          ac_out_ext_options_t *ext_options);
```
ac\_out\_ext\_init writes to a regular, partitioned, or sorted file.

```c
bool ac_out_write_record(ac_out_t *h, const void *d, size_t len);
```
ac\_out\_write\_record writes a record in the format specified by ac\_out\_options\_format(...)

```c
ac_in_t *ac_out_in(ac_out_t *h);
```
This only works if output is sorted.  This will bypass the writing of the final file and give you access to the cursor.

```c
void ac_out_destroy(ac_out_t *h);
```
ac\_out\_destroy destroys the ac\_out\_t object.

```c
bool ac_out_write(ac_out_t *h, const void *data, size_t len);
```
ac\_out\_write is only meant to be used when the ac\_out\_t is configured as a single file.  This will write data to the file which is len bytes.

```c
bool ac_out_write_prefix(ac_out_t *h, const void *data, size_t len);
```
ac\_out\_write\_prefix is only meant to be used when the ac\_out\_t is configured as a single file.  This will write a 4 byte length prefix and then the data.

```c
bool ac_out_write_delimiter(ac_out_t *h, const void *data, size_t len,
                            char delimiter);
```
ac\_out\_write\_delimiter is only meant to be used when the ac\_out\_t is configured as a single file.  This will write data, followed by a single byte delimiter.

```c
void ac_out_options_init(ac_out_options_t *h);
```
ac\_out\_options\_init initializes ac\_out\_options\_t to default values. ac\_out\_options\_t is declared in impl/ac\_out.h.  h is expected to point to a structure of this type (and not NULL).

```c
void ac_out_options_buffer_size(ac_out_options_t *h, size_t buffer_size);
```
ac\_out\_options\_buffer\_size sets the buffer size that the ac\_out handle has to use.

```c
void ac_out_options_format(ac_out_options_t *h, ac_io_format_t format);
```
ac\_out\_options\_format sets the format of the records that are written through ac\_out\_write\_record.  This should be called with one of the ac\_io\_format... methods.

Prefix format (4 byte length prefix before each record)<br/>
```ac_in_options_format(&options, ac_io_prefix());```

Delimiter format (specify a character at the end of a record)<br/>
```ac_in_options_format(&options, ac_io_delimiter('\n'));```

Fixed format (all records are the same length)<br/>
```ac_in_options_format(&options, ac_io_fixed(<some_length>));```

Other formats may be added in the future such as compressed, protobuf, etc.

```c
void ac_out_options_append_mode(ac_out_options_t *h);
```
ac\_out\_options\_append\_mode opens the file in append mode.  This currently does not work for lz4 files.

```c
void ac_out_options_safe_mode(ac_out_options_t *h);
```
ac\_out\_options\_safe\_mode writes the file with a -safe name and rename it upon completion.

```c
void ac_out_options_write_ack_file(ac_out_options_t *h);
```
ac\_out\_options\_write\_ack\_file will write an ack file after the ac\_out\_t has been destroyed..  This is useful if another program is picking up that the file is finished.

```c
void ac_out_options_gz(ac_out_options_t *h, int level);
```
ac\_out\_options\_gz sets the level of compression and identify the output as gzip if filename is not present.

```c
void ac_out_options_lz4(ac_out_options_t *h, int level,
                        ac_lz4_block_size_t size, bool block_checksum,
                        bool content_checksum);
```
ac\_out\_options\_lz4 sets the level of compression, the block size, whether block checksums are used, and content checksums.  The default is that the checksums are not used and block size is s64kb.  level defaults to 1.

```c
void ac_out_ext_options_init(ac_out_ext_options_t *h);
```
ac\_out\_ext\_options\_init initializes the ac\_out\_ext\_options\_t structure.  The extended options are for partitioned output, sorted output, or both.  

```c
void ac_out_ext_options_sort_before_partitioning(ac_out_ext_options_t *h);
```
Normally if data is partitioned and sorted, partitioning would happen first.  This has the added cost of writing the unsorted partitions.  Because the data is partitioned first, sorting can happen in parallel.  In some cases, it may be desirable to sort first and then partition the sorted data.  This option exists for those cases.

```c
void ac_out_ext_options_sort_while_partitioning(ac_out_ext_options_t *h);
```
Normally sorting will happen after partitions are written as more threads can be used for doing this.  However, sorting can occur while the partitions are being written out using this option.

```c
void ac_out_ext_options_num_sort_threads(ac_out_ext_options_t *h,
                                         size_t num_sort_threads);
```
when partitioning and sorting - how many partitions can be sorted at once?

```c
void ac_out_ext_options_partition(ac_out_ext_options_t *h,
                                  ac_io_partition_f part, void *arg);
```
options for creating a partitioned output

```c
void ac_out_ext_options_num_partitions(ac_out_ext_options_t *h,
                                       size_t num_partitions);
```

```c
void ac_out_ext_options_intermediate_group_size(ac_out_ext_options_t *h,
                                                size_t num_per_group);
```
By default, tmp files are written every time the buffer fills and all of the tmp files are merged at the end.  This causes the tmp files to be merged once the number of tmp files reaches the num\_per\_group.

```c
void ac_out_ext_options_compare(ac_out_ext_options_t *h,
                                ac_io_compare_f compare, void *arg);
```
options for comparing output

```c
void ac_out_ext_options_intermediate_compare(ac_out_ext_options_t *h,
                                             ac_io_compare_f compare,
                                             void *arg);
```

```c
void ac_out_ext_options_reducer(ac_out_ext_options_t *h,
                                ac_io_reducer_f reducer, void *arg);
```
set the reducers

```c
void ac_out_ext_options_intermediate_reducer(ac_out_ext_options_t *h,
                                             ac_io_reducer_f reducer,
                                             void *arg);
```
Overrides the reducer specified by ac\_out\_ext\_options\_reducer for internal reducing of file.

```c
void ac_out_ext_options_use_extra_thread(ac_out_ext_options_t *h);
```
Use an extra thread when sorting output.

```c
void ac_out_ext_options_dont_compress_tmp(ac_out_ext_options_t *h);
```
Default tmp files are stored in lz4 format.  Disable this behavior.

```c
void ac_out_partition_filename(char *dest, const char *filename, size_t id);
```
used to create a partitioned filename
