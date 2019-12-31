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

void ac_out_init_options(ac_out_options_t *r);
void ac_out_fd(ac_out_options_t *r, int fd, bool owner);
void ac_out_buffer_size(ac_out_options_t *r, size_t buffer_size);
void ac_out_format(ac_out_options_t *r, ac_io_format_t format);
void ac_out_abort_on_error(ac_out_options_t *r);
void ac_out_append_mode(ac_out_options_t *r);
void ac_out_safe_mode(ac_out_options_t *r);
void ac_out_write_ack_file(ac_out_options_t *r);
void ac_out_gz(ac_out_options_t *r, int level);
void ac_out_lz4(ac_out_options_t *r, int level, ac_lz4_block_size_t size,
                bool block_checksum, bool content_checksum);

ac_out_t *ac_out_init(const char *filename, ac_out_options_t *options);

bool ac_out_write(ac_out_t *h, const void *d, size_t len);
bool ac_out_write_prefix(ac_out_t *h, const void *d, size_t len);
bool ac_out_write_delimiter(ac_out_t *h, const void *d, size_t len,
                            char delimiter);
bool ac_out_write_record(ac_out_t *h, const void *d, size_t len);

void ac_out_destroy(ac_out_t *h);

#endif
