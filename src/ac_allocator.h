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

#ifndef _ac_allocator_H
#define _ac_allocator_H

#include "ac_common.h"

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
#define ac_free(p) _ac_free_d(NULL, __AC_FILE_LINE__, p)
#else
#define ac_malloc(len) malloc(len)
#define ac_calloc(len) calloc(1, len)
#define ac_realloc(p, len) realloc(p, len)
#define ac_strdup(p) strdup(p)
#define ac_free(p) free(p)
#endif

typedef void (*ac_dump_details_f)(FILE *out, const char *caller, void *p,
                                  size_t length);

typedef struct {
  ac_dump_details_f dump;
} ac_allocator_dump_t;

struct ac_allocator_s;
typedef struct ac_allocator_s ac_allocator_t;

ac_allocator_t *ac_allocator_init(const char *filename, bool thread_safe);
void ac_allocator_destroy(ac_allocator_t *a);

void ac_dump_global_allocations(ac_allocator_t *a, FILE *out);

void *_ac_malloc_d(ac_allocator_t *a, const char *caller, size_t len,
                   bool custom);

void *_ac_calloc_d(ac_allocator_t *a, const char *caller, size_t len,
                   bool custom);

void *_ac_realloc_d(ac_allocator_t *a, const char *caller, void *p, size_t len,
                    bool custom);

char *_ac_strdup_d(ac_allocator_t *a, const char *caller, const char *p);

void _ac_free_d(ac_allocator_t *a, const char *caller, void *p);

#ifdef __cplusplus
}
#endif

#endif
