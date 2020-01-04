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

#ifndef _ac_io_H
#define _ac_io_H

#include "ac_buffer.h"
#include "ac_common.h"

#include <inttypes.h>

typedef int ac_io_format_t;

ac_io_format_t ac_io_delimiter(int delim);
ac_io_format_t ac_io_fixed(int size);
ac_io_format_t ac_io_prefix();

typedef struct {
  char *record;
  uint32_t length;
  int32_t tag;
} ac_io_record_t;

typedef bool (*ac_io_reducer_f)(ac_io_record_t *res, const ac_io_record_t *r,
                                size_t num_r, ac_buffer_t *bh, void *tag);

typedef int (*ac_io_compare_f)(const ac_io_record_t *, const ac_io_record_t *,
                               void *tag);

typedef size_t (*ac_io_partition_f)(const ac_io_record_t *r, size_t num_part,
                                    void *tag);

typedef bool (*ac_io_fixed_reducer_f)(char *d, size_t num_r, void *tag);

typedef void (*ac_io_fixed_sort_f)(void *p, size_t total_elems);

typedef int (*ac_io_fixed_compare_f)(const void *p1, const void *p2, void *tag);

bool ac_io_keep_first(ac_io_record_t *res, const ac_io_record_t *r,
                      size_t num_r, ac_buffer_t *bh, void *tag);

size_t ac_io_hash_partition(const ac_io_record_t *r, size_t num_part,
                            void *tag);

size_t ac_io_file_size(const char *filename);

/* char *ac_io_read_file(size_t *len, const char *filename); */

#ifdef _AC_DEBUG_MEMORY_
#define ac_io_read_file(len, filename)                                         \
  _ac_io_read_file(len, filename, AC_FILE_LINE_MACRO("ac_io_read_file"))
char *_ac_io_read_file(size_t *len, const char *filename, const char *caller);
#else
#define ac_io_read_file(len, filename) _ac_io_read_file(len, filename)
char *_ac_io_read_file(size_t *len, const char *filename);
#endif

/*
  Make the given directory if it doesn't already exist.  Return false if an
  error occurred.
*/
bool ac_io_make_directory(const char *path);

/*
   This will take a full filename and temporarily place a \0 in place of the
   last slash (/).  If there isn't a slash, then it will return true. Otherwise,
   it will check to see if path exists and create it if it does not exist.  If
   an error occurs, false will be returned.
*/
bool ac_io_make_path_valid(char *filename);

/*
  test if filename has extension, extension is expected to have . (ex - ".lz4")
  If filename is NULL, false will be returned.
*/
bool ac_io_extension(const char *filename, const char *extension);

#endif
