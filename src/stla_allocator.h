#ifndef _stla_allocator_H
#define _stla_allocator_H

#include "stla_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _STLA_DEBUG_MEMORY_
#define stla_malloc(len) _stla_malloc_d(NULL, __STLA_FILE_LINE__, len, false)
#define stla_calloc(len) _stla_calloc_d(NULL, __STLA_FILE_LINE__, len, false)
#define stla_realloc(p, len) _stla_realloc_d(NULL, __STLA_FILE_LINE__, p, len, false)
#define stla_strdup(p) _stla_strdup_d(NULL, __STLA_FILE_LINE__, p)
#define stla_free(p) _stla_free_d(NULL, __STLA_FILE_LINE__, p)
#else
#define stla_malloc(len) malloc(len)
#define stla_calloc(len) calloc(1, len)
#define stla_realloc(p, len) realloc(p, len)
#define stla_strdup(p) strdup(p)
#define stla_free(p) free(p)
#endif

typedef void (*stla_dump_details_f)(FILE *out, const char *caller, void *p, size_t length);

typedef struct {
  stla_dump_details_f dump;
} stla_allocator_dump_t;

struct stla_allocator_s;
typedef struct stla_allocator_s stla_allocator_t;

stla_allocator_t *stla_allocator_init(const char *filename, bool thread_safe);
void stla_allocator_destroy(stla_allocator_t *a);

void stla_dump_global_allocations(stla_allocator_t *a, FILE *out);

void *_stla_malloc_d(stla_allocator_t *a, const char *caller, size_t len, bool custom );

void *_stla_calloc_d(stla_allocator_t *a, const char *caller, size_t len, bool custom );

void *_stla_realloc_d(stla_allocator_t *a, const char *caller, void *p, size_t len, bool custom);

char *_stla_strdup_d(stla_allocator_t *a, const char *caller, const char *p );

void _stla_free_d(stla_allocator_t *a, const char *caller, void *p);

#endif
