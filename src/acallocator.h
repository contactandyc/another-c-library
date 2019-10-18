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

#ifndef _acallocator_H
#define _acallocator_H

#include "accommon.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _ACDEBUG_MEMORY_
#define acmalloc(len) _acmalloc_d(NULL, __ACFILE_LINE__, len, false)
#define accalloc(len) _accalloc_d(NULL, __ACFILE_LINE__, len, false)
#define acrealloc(p, len)                                                   \
  _acrealloc_d(NULL, __ACFILE_LINE__, p, len, false)
#define acstrdup(p) _acstrdup_d(NULL, __ACFILE_LINE__, p)
#define acfree(p) _acfree_d(NULL, __ACFILE_LINE__, p)
#else
#define acmalloc(len) malloc(len)
#define accalloc(len) calloc(1, len)
#define acrealloc(p, len) realloc(p, len)
#define acstrdup(p) strdup(p)
#define acfree(p) free(p)
#endif

typedef void (*acdump_details_f)(FILE *out, const char *caller, void *p,
                                    size_t length);

typedef struct {
  acdump_details_f dump;
} acallocator_dump_t;

struct acallocator_s;
typedef struct acallocator_s acallocator_t;

acallocator_t *acallocator_init(const char *filename, bool thread_safe);
void acallocator_destroy(acallocator_t *a);

void acdump_global_allocations(acallocator_t *a, FILE *out);

void *_acmalloc_d(acallocator_t *a, const char *caller, size_t len,
                     bool custom);

void *_accalloc_d(acallocator_t *a, const char *caller, size_t len,
                     bool custom);

void *_acrealloc_d(acallocator_t *a, const char *caller, void *p,
                      size_t len, bool custom);

char *_acstrdup_d(acallocator_t *a, const char *caller, const char *p);

void _acfree_d(acallocator_t *a, const char *caller, void *p);

#endif
