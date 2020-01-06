/*
Copyright 2019 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _ac_out_H
#define _ac_out_H

#include "ac_common.h"
#include "ac_io.h"
#include "ac_lz4.h"

#include "impl/ac_out.h"

#include "ac_in.h"

/* ac_out_options_t is declared in impl/ac_out.h.  h is expected to point to a
   structure of this type (and not NULL). */
void ac_out_options_init(ac_out_options_t *h);
void ac_out_options_buffer_size(ac_out_options_t *h, size_t buffer_size);
void ac_out_options_format(ac_out_options_t *h, ac_io_format_t format);
void ac_out_options_abort_on_error(ac_out_options_t *h);
void ac_out_options_append_mode(ac_out_options_t *h);
void ac_out_options_safe_mode(ac_out_options_t *h);
void ac_out_options_write_ack_file(ac_out_options_t *h);
void ac_out_options_gz(ac_out_options_t *h, int level);
void ac_out_options_lz4(ac_out_options_t *h, int level,
                        ac_lz4_block_size_t size, bool block_checksum,
                        bool content_checksum);

/* extended options are for partitioned output, sorted output, or both */
void ac_out_ext_options_init(ac_out_ext_options_t *h);

/* Normally if data is partitioned and sorted, partitioning would happen first.
   This has the added cost of writing the unsorted partitions.  Because the
   data is partitioned first, sorting can happen in parallel.  In some cases,
   it may be desirable to sort first and then partition the sorted data.  This
   option exists for those cases. */
void ac_out_ext_options_sort_before_partitioning(ac_out_ext_options_t *h);

/* Normally sorting will happen after partitions are written as more threads
   can be used for doing this.  However, sorting can occur while the partitions
   are being written out using this option. */
void ac_out_ext_options_sort_while_partitioning(ac_out_ext_options_t *h);

/* when partitioning and sorting - how many partitions can be sorted at once? */
void ac_out_ext_options_num_sort_threads(ac_out_ext_options_t *h,
                                         size_t num_sort_threads);

/* options for creating a partitioned output */
void ac_out_ext_options_partition(ac_out_ext_options_t *h,
                                  ac_io_partition_f part, void *tag);

void ac_out_ext_options_num_partitions(ac_out_ext_options_t *h,
                                       size_t num_partitions);

/* By default, tmp files are written every time the buffer fills and all of the
   tmp files are merged at the end.  This causes the tmp files to be merged
   once the number of tmp files reaches the num_per_group. */
void ac_out_ext_options_intermediate_group_size(ac_out_ext_options_t *h,
                                                size_t num_per_group);

/* options for comparing output */
void ac_out_ext_options_compare(ac_out_ext_options_t *h,
                                ac_io_compare_f compare, void *tag);

void ac_out_ext_options_intermediate_compare(ac_out_ext_options_t *h,
                                             ac_io_compare_f compare,
                                             void *tag);

/* set the reducers */
void ac_out_ext_options_reducer(ac_out_ext_options_t *h,
                                ac_io_reducer_f reducer, void *tag);

void ac_out_ext_options_intermediate_reducer(ac_out_ext_options_t *h,
                                             ac_io_reducer_f reducer,
                                             void *tag);

void ac_out_ext_options_use_extra_thread(ac_out_ext_options_t *h);

void ac_out_ext_options_dont_compress_tmp(ac_out_ext_options_t *h);

/* open a file for writing with a given filename */
ac_out_t *ac_out_init(const char *filename, ac_out_options_t *options);

/* use an open file for writing.  If fd_owner is true, the file descriptor
   will be closed when this is destroyed. */
ac_out_t *ac_out_init_with_fd(int fd, bool fd_owner, ac_out_options_t *options);

/* write to a regular, partitioned, or sorted file. */
ac_out_t *ac_out_ext_init(const char *filename, ac_out_options_t *options,
                          ac_out_ext_options_t *ext_options);

/* write record in the format specified by ac_out_options_format(...) */
bool ac_out_write_record(ac_out_t *h, const void *d, size_t len);

/* This only works if output is sorted.  This will bypass the writing of the
   final file and give you access to the cursor. */
ac_in_t *ac_out_in(ac_out_t *h);

/* destroy the output. */
void ac_out_destroy(ac_out_t *h);

/* these methods only work if writing to a single file */
bool ac_out_write(ac_out_t *h, const void *d, size_t len);
bool ac_out_write_prefix(ac_out_t *h, const void *d, size_t len);
bool ac_out_write_delimiter(ac_out_t *h, const void *d, size_t len,
                            char delimiter);

#endif
