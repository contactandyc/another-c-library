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

#ifndef _ac_pool_H
#define _ac_pool_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "ac_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ac_pool_s;
typedef struct ac_pool_s ac_pool_t;

/* ac_pool_init will create a working space of size bytes */
#ifdef _AC_DEBUG_MEMORY_
#define ac_pool_init(size) _ac_pool_init(size, AC_FILE_LINE_MACRO("ac_pool"))
ac_pool_t *_ac_pool_init(size_t size, const char *caller);
#else
#define ac_pool_init(size) _ac_pool_init(size)
ac_pool_t *_ac_pool_init(size_t size);
#endif

/* ac_pool_clear will make all of the pool's memory reusable.  If the
  initial block was exceeded and additional blocks were added, those blocks
  will be freed. */
void ac_pool_clear(ac_pool_t *h);

struct ac_pool_checkpoint_s;
typedef struct ac_pool_checkpoint_s ac_pool_checkpoint_t;

static inline void ac_pool_checkpoint(ac_pool_t *h, ac_pool_checkpoint_t *cp);
static inline void ac_pool_reset(ac_pool_t *h, ac_pool_checkpoint_t *cp);

/* ac_pool_destroy frees up all memory associated with the pool object */
void ac_pool_destroy(ac_pool_t *h);

/* ac_pool_set_minimum_growth_size alters the minimum size of growth blocks.
   This is particularly useful if you don't expect the pool's block size to be
   exceeded by much and you don't want the default which would be to use the
   original block size for the new block (effectively doubling memory usage). */
void ac_pool_set_minimum_growth_size(ac_pool_t *h, size_t size);

/* ac_pool_alloc allocates len uninitialized bytes which are aligned. */
static inline void *ac_pool_alloc(ac_pool_t *h, size_t len);

/* ac_pool_min_max_alloc allocates at least min_len bytes and up to len bytes.
   If the */
static inline void *ac_pool_min_max_alloc(ac_pool_t *h, size_t *rlen,
                                          size_t min_len, size_t len);

/* ac_pool_alloc allocates len uninitialized bytes which are unaligned. */
static inline void *ac_pool_ualloc(ac_pool_t *h, size_t len);

/* ac_pool_alloc allocates len zero'd bytes which are aligned. */
static inline void *ac_pool_calloc(ac_pool_t *h, size_t len);

/* ac_pool_strdup allocates a copy of the string p.  The memory will be
  unaligned.  If you need the memory to be aligned, consider using ac_pool_dup
  like char *s = ac_pool_dup(pool, p, strlen(p)+1); */
static inline char *ac_pool_strdup(ac_pool_t *h, const char *p);

/* like ac_pool_strdup, limited to length (+1 for zero terminator) bytes */
static inline char *ac_pool_strndup(ac_pool_t *h, const char *p, size_t length);

/* ac_pool_dup allocates a copy of the data.  The memory will be aligned. */
static inline void *ac_pool_dup(ac_pool_t *h, const void *data, size_t len);

/* ac_pool_dup allocates a copy of the data.  The memory will be unaligned. */
static inline void *ac_pool_udup(ac_pool_t *h, const void *data, size_t len);

/* ac_pool_strdupf allocates a copy of the formatted string p. */
static inline char *ac_pool_strdupf(ac_pool_t *h, const char *p, ...);

/* ac_pool_strdupvf allocates a copy of the formatted string p. This is
  particularly useful if you wish to extend another object which uses pool as
  its base.  */
char *ac_pool_strdupvf(ac_pool_t *h, const char *p, va_list args);

/* ac_pool_size returns the number of bytes that have been allocated from any
  of the alloc calls above.  */
size_t ac_pool_size(ac_pool_t *h);

/* ac_pool_used returns the number of bytes that have been allocated by the
  pool itself.  This will always be greater than ac_pool_size as there is
  overhead for the structures and this is independent of any allocating calls.
*/
size_t ac_pool_used(ac_pool_t *h);

#include "impl/ac_pool.h"

#ifdef __cplusplus
}
#endif

#endif
