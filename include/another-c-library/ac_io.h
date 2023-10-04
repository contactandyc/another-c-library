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

#include "another-c-library/ac_buffer.h"
#include "another-c-library/ac_common.h"
#include "the-macro-library/macro_sort.h"

#include <inttypes.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int ac_io_format_t;

ac_io_format_t ac_io_delimiter(int delim);
ac_io_format_t ac_io_csv_delimiter(int delim);
ac_io_format_t ac_io_fixed(int size);
ac_io_format_t ac_io_prefix();

typedef struct {
  char *record;
  uint32_t length;
  int32_t tag;
} ac_io_record_t;

typedef struct ac_io_file_info_s {
  char *filename;
  size_t size;
  time_t last_modified;
  uint64_t hash;
  int32_t tag;
} ac_io_file_info_t;

_macro_sort_compare_h(ac_io_sort_records, cmp_arg, ac_io_record_t);

typedef bool (*ac_io_reducer_cb)(ac_io_record_t *res, const ac_io_record_t *r,
                                size_t num_r, ac_buffer_t *bh, void *tag);

typedef int (*ac_io_compare_cb)(const ac_io_record_t *, const ac_io_record_t *,
                               void *tag);

typedef size_t (*ac_io_partition_cb)(const ac_io_record_t *r, size_t num_part,
                                    void *tag);

typedef bool (*ac_io_fixed_reducer_cb)(char *d, size_t num_r, void *tag);

typedef void (*ac_io_fixed_sort_cb)(void *p, size_t total_elems);

typedef int (*ac_io_fixed_compare_cb)(const void *p1, const void *p2, void *tag);

typedef bool (*ac_file_valid_cb)(const char *filename, void *arg);

bool ac_io_keep_first(ac_io_record_t *res, const ac_io_record_t *r,
                      size_t num_r, ac_buffer_t *bh, void *tag);

size_t ac_io_hash_partition(const ac_io_record_t *r, size_t num_part,
                            void *tag);

bool ac_io_file_info(ac_io_file_info_t *fi);

ac_io_file_info_t *
ac_io_list(const char *path, size_t *num_files,
           ac_file_valid_cb file_valid, void *arg);

ac_io_file_info_t *
ac_pool_io_list(ac_pool_t *pool, const char *path, size_t *num_files,
                bool (*file_valid)(const char *filename, void *arg), void *arg);

ac_io_file_info_t *ac_io_select_file_info(ac_pool_t *pool, size_t *num_res,
                                          ac_io_file_info_t *inputs,
                                          size_t num_inputs, size_t partition,
                                          size_t num_partitions);

void ac_io_sort_file_info_by_last_modified(ac_io_file_info_t *files, size_t num_files);

bool ac_io_file_exists(const char *filename);

size_t ac_io_file_size(const char *filename);

time_t ac_io_modified(const char *filename);

bool ac_io_directory(const char *filename);
bool ac_io_file(const char *filename);

/* Read the contents of filename into a buffer and return it's length.  The
   buffer should be freed using ac_free.

  char *ac_io_read_file(size_t *len, const char *filename); */

#ifdef _AC_MEMORY_CHECK_
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
  test if filename has extension, (ex - "lz4", "" if no extension expected)
  If filename is NULL, false will be returned.
*/
bool ac_io_extension(const char *filename, const char *extension);

/* Do not put on website - common comparison and split functions */
static inline int ac_io_compare_uint64_t(const ac_io_record_t *p1,
                                         const ac_io_record_t *p2, void *tag) {
  uint64_t *a = (uint64_t *)p1->record;
  uint64_t *b = (uint64_t *)p2->record;
  if (*a != *b)
    return (*a < *b) ? -1 : 1;
  return 0;
}

static inline int ac_io_compare_uint32_t(const ac_io_record_t *p1,
                                         const ac_io_record_t *p2, void *tag) {
  uint32_t *a = (uint32_t *)p1->record;
  uint32_t *b = (uint32_t *)p2->record;
  if (*a != *b)
    return (*a < *b) ? -1 : 1;
  return 0;
}

static inline size_t ac_io_split_by_uint64_t(const ac_io_record_t *r,
                                             size_t num_part, void *tag) {
  uint64_t *a = (uint64_t *)r->record;
  return (*a) % num_part;
}

static inline size_t ac_io_split_by_uint32_t(const ac_io_record_t *r,
                                             size_t num_part, void *tag) {
  uint32_t *a = (uint32_t *)r->record;
  uint32_t np = num_part;
  return (*a) % np;
}

static inline size_t ac_io_split_by_uint64_t_2(const ac_io_record_t *r,
                                               size_t num_part, void *tag) {
  uint64_t *a = (uint64_t *)r->record;
  return (a[1]) % num_part;
}

static inline size_t ac_io_split_by_uint32_t_2(const ac_io_record_t *r,
                                               size_t num_part, void *tag) {
  uint32_t *a = (uint32_t *)r->record;
  uint32_t np = num_part;
  return (a[1]) % np;
}

#ifdef __cplusplus
}
#endif

#endif
