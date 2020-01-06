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

/* ac_out_options_t is declared in impl/ac_out.h.  r is expected to point to a
   structure of this type (and not NULL). */
void ac_out_options_init(ac_out_options_t *h);
void ac_out_options_fd(ac_out_options_t *h, int fd, bool owner);
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

/* options for creating a partitioned output */
void ac_out_ext_options_partition(ac_out_ext_options_t *h,
                                  ac_io_partition_f part, void *tag);

void ac_out_ext_options_num_partitions(ac_out_ext_options_t *h,
                                       size_t num_partitions);

/* options for sorting the output */
void ac_out_ext_options_compare(ac_out_ext_options_t *h,
                                ac_io_compare_f compare, void *tag);

void ac_out_ext_options_intermediate_group_size(ac_out_ext_options_t *h,
                                                size_t num_per_group);

void ac_out_ext_options_intermediate_compare(ac_out_ext_options_t *h,
                                             ac_io_compare_f compare,
                                             void *tag);

/* set the reducer */
void ac_out_ext_options_reducer(ac_out_ext_options_t *h,
                                ac_io_reducer_f reducer, void *tag);

/* options for fixed output */
void ac_out_ext_options_fixed_reducer(ac_out_ext_options_t *h,
                                      ac_io_fixed_reducer_f reducer, void *tag);
void ac_out_ext_options_fixed_compare(ac_out_ext_options_t *h,
                                      ac_io_fixed_compare_f compare, void *tag);
void ac_out_ext_options_fixed_sort(ac_out_ext_options_t *h,
                                   ac_io_fixed_sort_f sort, void *tag);

ac_out_t *ac_out_init(const char *filename, ac_out_options_t *options);
ac_out_t *ac_out_ext_init(const char *filename, ac_out_options_t *options,
                          ac_out_ext_options_t *ext_options);

bool ac_out_write_record(ac_out_t *h, const void *d, size_t len);

void ac_out_tag(ac_out_t *h, int tag);
ac_in_t *ac_out_in(ac_out_t *h);

void ac_out_destroy(ac_out_t *h);

/* these methods only work if writing to a single file */
bool ac_out_write(ac_out_t *h, const void *d, size_t len);
bool ac_out_write_prefix(ac_out_t *h, const void *d, size_t len);
bool ac_out_write_delimiter(ac_out_t *h, const void *d, size_t len,
                            char delimiter);

#endif
