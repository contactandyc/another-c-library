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

/*
  Memory errors are common in C applications.  The ac_allocator attempts to
  mitigate a few of the most common ones such as freeing the wrong address,
  freeing memory more than once, and forgetting to free memory.  If the
  allocator is used in debug mode, allocations will be tracked.  In release
  mode, the functions all compile down to the system equivalents.
*/

#ifndef _ac_allocator_H
#define _ac_allocator_H

#include "ac_common.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _AC_DEBUG_MEMORY_
#define ac_malloc(len) _ac_malloc_d(NULL, __AC_FILE_LINE__, len, false)
#define ac_calloc(len) _ac_calloc_d(NULL, __AC_FILE_LINE__, len, false)
#define ac_realloc(p, len) _ac_realloc_d(NULL, __AC_FILE_LINE__, p, len, false)
#define ac_strdup(p) _ac_strdup_d(NULL, __AC_FILE_LINE__, p)
#define ac_strdupf(p, ...) _ac_strdupf_d(NULL, __AC_FILE_LINE__, p, __VA_ARGS__)
#define ac_strdupvf(p, args) _ac_strdupvf_d(NULL, __AC_FILE_LINE__, p, args)
#define ac_strdupa(p) _ac_strdupa_d(NULL, __AC_FILE_LINE__, p)
#define ac_strdupan(p, n) _ac_strdupan_d(NULL, __AC_FILE_LINE__, p, n)
#define ac_strdupa2(p) _ac_strdupa2_d(NULL, __AC_FILE_LINE__, p)
#define ac_memdup(p, len) _ac_memdup_d(NULL, __AC_FILE_LINE__, p, len)
#define ac_split(num_splits, delim, s)                                         \
  _ac_split_d(NULL, __AC_FILE_LINE__, num_splits, delim, s)
#define ac_split2(num_splits, delim, s)                                        \
  _ac_split2_d(NULL, __AC_FILE_LINE__, num_splits, delim, s)
#define ac_free(p) _ac_free_d(NULL, __AC_FILE_LINE__, p)
#else
#define ac_malloc(len) malloc(len)
#define ac_calloc(len) calloc(1, len)
#define ac_realloc(p, len) realloc(p, len)
#define ac_strdup(p) strdup(p)
#define ac_strdupf(p, ...) _ac_strdupf(p, __VA_ARGS__)
#define ac_strdupvf(p, args) _ac_strdupvf(p, args)
#define ac_strdupa(p) _ac_strdupa(p)
#define ac_strdupan(p, n) _ac_strdupan(p, n)
#define ac_strdupa2(p) _ac_strdupa2(p)
#define ac_memdup(p, len) _ac_memdup(p, len)
#define ac_split(num_splits, delim, s)                                         \
  _ac_split_d(NULL, NULL, num_splits, delim, s)
#define ac_split2(num_splits, delim, s)                                        \
  _ac_split2_d(NULL, NULL, num_splits, delim, s)
#define ac_free(p) free(p)
#endif

typedef void (*ac_dump_details_cb)(FILE *out, const char *caller, void *p,
                                  size_t length);

typedef struct {
  ac_dump_details_cb dump;
} ac_allocator_dump_t;

struct ac_allocator_s;
typedef struct ac_allocator_s ac_allocator_t;

ac_allocator_t *ac_allocator_init(const char *filename, bool thread_safe);
void ac_allocator_destroy(ac_allocator_t *a);

char *_ac_strdupf_d(ac_allocator_t *a, const char *caller, const char *p, ...);
char *_ac_strdupf(const char *p, ...);
char *_ac_strdupvf_d(ac_allocator_t *a, const char *caller, const char *p,
                     va_list args);
char *_ac_strdupvf(const char *p, va_list args);

char **_ac_strdupa_d(ac_allocator_t *al, const char *caller, char **a);
char **_ac_strdupan_d(ac_allocator_t *al, const char *caller, char **a,
                      size_t n);
char **_ac_strdupa(char **a);
char **_ac_strdupan(char **a, size_t n);

char **_ac_strdupa2_d(ac_allocator_t *al, const char *caller, char **a);
char **_ac_strdupa2(char **a);

void ac_dump_global_allocations(ac_allocator_t *a, FILE *out);

void *_ac_malloc_d(ac_allocator_t *a, const char *caller, size_t len,
                   bool custom);

void *_ac_calloc_d(ac_allocator_t *a, const char *caller, size_t len,
                   bool custom);

void *_ac_realloc_d(ac_allocator_t *a, const char *caller, void *p, size_t len,
                    bool custom);

char *_ac_strdup_d(ac_allocator_t *a, const char *caller, const char *p);

void _ac_free_d(ac_allocator_t *a, const char *caller, void *p);

char **_ac_split_d(ac_allocator_t *a, const char *caller, size_t *num_splits,
                   char delim, const char *s);

char **_ac_split2_d(ac_allocator_t *a, const char *caller, size_t *num_splits,
                    char delim, const char *s);

static inline void *_ac_memdup_d(ac_allocator_t *a, const char *caller,
                                 const void *p, size_t len) {
  void *r = _ac_malloc_d(a, caller, len, false);
  memcpy(r, p, len);
  return r;
}

static inline void *_ac_memdup(const void *p, size_t len) {
  void *r = malloc(len);
  memcpy(r, p, len);
  return r;
}

#ifdef __cplusplus
}
#endif

#endif
