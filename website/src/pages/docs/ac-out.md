---
title: ac_out
description:
---

```c
#include "ac_out.h"
```

## ac_out_init

```c
ac_out_t *ac_out_init(const char *filename, ac_out_options_t *options);
```

`ac_out_init` opens a file for writing with a given filename

## ac_out_init_with_fd

```c
ac_out_t *ac_out_init_with_fd(int fd, bool fd_owner, ac_out_options_t *options);
```

`ac_out_init_with_fd` associates an open file descriptor to an `ac_out` object. If `fd_owner` is true, the file descriptor will be closed when this is destroyed.

## ac_out_ext_init

```c
ac_out_t *ac_out_ext_init(const char *filename, ac_out_options_t *options,
                          ac_out_ext_options_t *ext_options);
```
`ac_out_ext_init` writes to a regular, partitioned, or sorted file.

## ac_out_write_record

```c
bool ac_out_write_record(ac_out_t *h, const void *d, size_t len);
```

`ac_out_write_record` writes a record in the format specified by `ac_out_options_format(...)`

## ac_out_in

```c
ac_in_t *ac_out_in(ac_out_t *h);
```
`ac_out_in` converts an output into an input. This will bypass the writing of the final file and give you access to the cursor. Once this is called, `ac_out_destroy` should not be called.

## ac_out_destroy

```c
void ac_out_destroy(ac_out_t *h);
```
`ac_out_destroy` destroys the `ac_out_t` object.

## ac_out_write

```c
bool ac_out_write(ac_out_t *h, const void *data, size_t len);
```

`ac_out_write` is only meant to be used when the `ac_out_t` is configured as a single file. This will write data to the file which is len bytes.

## ac_out_write_prefix

```c
bool ac_out_write_prefix(ac_out_t *h, const void *data, size_t len);
```

`ac_out_write_prefix` is only meant to be used when the `ac_out_t` is configured as a single file. This will write a 4 byte length prefix and then the data.

## ac_out_write_delimiter

```c
bool ac_out_write_delimiter(ac_out_t *h, const void *data, size_t len,
                            char delimiter);
```

`ac_out_write_delimiter` is only meant to be used when the `ac_out_t` is configured as a single file. This will write data, followed by a single byte delimiter.

## ac_out_options_init

```c
void ac_out_options_init(ac_out_options_t *h);
```

`ac_out_options_init` initializes `ac_out_options_t` to default values. `ac_out_options_t` is declared in `impl/ac_out.h`. h is expected to point to a structure of this type (and not NULL).

## ac_out_options_buffer_size

```c
void ac_out_options_buffer_size(ac_out_options_t *h, size_t buffer_size);
```

`ac_out_options_buffer_size` sets the buffer size that the `ac_out` handle has to use.

## ac_out_options_format

```c
void ac_out_options_format(ac_out_options_t *h, ac_io_format_t format);
```

`ac_out_options_format` sets the format of the records that are written through `ac_out_write_record`. This should be called with one of the `ac_io_format...` methods.

Prefix format (4 byte length prefix before each record)

```ac_in_options_format(&options, ac_io_prefix());```

Delimiter format (specify a character at the end of a record)

```ac_in_options_format(&options, ac_io_delimiter('\n'));```

Fixed format (all records are the same length)

```ac_in_options_format(&options, ac_io_fixed(<some_length>));```

Other formats may be added in the future such as compressed, protobuf, etc.

## ac_out_options_append_mode

```c
void ac_out_options_append_mode(ac_out_options_t *h);
```

`ac_out_options_append_mode` opens the file in append mode. This currently does not work for lz4 files.

## ac_out_options_safe_mode

```c
void ac_out_options_safe_mode(ac_out_options_t *h);
```

`ac_out_options_safe_mode` writes the file with a -safe name and rename it upon completion.

## ac_out_options_write_ack_file

```c
void ac_out_options_write_ack_file(ac_out_options_t *h);
```

`ac_out_options_write_ack_file` will write an ack file after the `ac_out_t` has been destroyed.. This is useful if another program is picking up that the file is finished.

## ac_out_options_gz

```c
void ac_out_options_gz(ac_out_options_t *h, int level);
```

`ac_out_options_gz` sets the level of compression and identify the output as gzip if filename is not present.

## ac_out_options_lz4

```c
void ac_out_options_lz4(ac_out_options_t *h, int level,
                        ac_lz4_block_size_t size, bool block_checksum,
                        bool content_checksum);
```

`ac_out_options_lz4` sets the level of compression, the block size, whether block checksums are used, and content checksums. The default is that the checksums are not used and block size is s64kb. level defaults to 1.

## ac_out_ext_options_init

```c
void ac_out_ext_options_init(ac_out_ext_options_t *h);
```

`ac_out_ext_options_init` initializes the `ac_out_ext_options_t` structure. The extended options are for partitioned output, sorted output, or both. 

## ac_out_ext_options_sort

```c
void ac_out_ext_options_sort_before_partitioning(ac_out_ext_options_t *h);
```

Normally if data is partitioned and sorted, partitioning would happen first. This has the added cost of writing the unsorted partitions. Because the data is partitioned first, sorting can happen in parallel. In some cases, it may be desirable to sort first and then partition the sorted data. This option exists for those cases.

```c
void ac_out_ext_options_sort_while_partitioning(ac_out_ext_options_t *h);
```

Normally sorting will happen after partitions are written as more threads can be used for doing this. However, sorting can occur while the partitions are being written out using this option.

## ac_out_ext_options_num_

```c
void ac_out_ext_options_num_sort_threads(ac_out_ext_options_t *h,
                                         size_t num_sort_threads);
```

when partitioning and sorting - how many partitions can be sorted at once?

## ac_out_ext_options_partition

```c
void ac_out_ext_options_partition(ac_out_ext_options_t *h,
                                  ac_io_partition_f part, void *arg);
```

options for creating a partitioned output

## ac_out_ext_options_num_partitions

```c
void ac_out_ext_options_num_partitions(ac_out_ext_options_t *h,
                                       size_t num_partitions);
```

## ac_out_ext_options_int

```c
void ac_out_ext_options_intermediate_group_size(ac_out_ext_options_t *h,
                                                size_t num_per_group);
```

By default, tmp files are written every time the buffer fills and all of the tmp files are merged at the end. This causes the tmp files to be merged once the number of tmp files reaches the `num_per_group`.

## ac_out_ext_options_compare

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

## ac_out_ext_options_reducer

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

Overrides the reducer specified by `ac_out_ext_options_reducer` for internal reducing of file.

```c
void ac_out_ext_options_use_extra_thread(ac_out_ext_options_t *h);
```

Use an extra thread when sorting output.

```c
void ac_out_ext_options_dont_compress_tmp(ac_out_ext_options_t *h);
```

Default tmp files are stored in lz4 format. Disable this behavior.

## ac_out_partition_filename

```c
void ac_out_partition_filename(char *dest, const char *filename, size_t id);
```

used to create a partitioned filename

<NextPrev prev="ac_map_reduce Part 3" prevUrl="/docs/ac-map-reduce-3" next="ac_pool" nextUrl="/docs/ac-pool" />
