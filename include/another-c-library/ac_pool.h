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
  The ac_pool provides an api similar to malloc, calloc, strdup, along with many
  other useful common allocation patterns with the exception of free.  The pool
  must be cleared (ac_pool_clear) or destroyed (ac_pool_destroy) to reclaim
  memory allocated.

  C doesn't have garbage collection.  Many C developers prefer to stay away from
  languages which have it as it can cause performance issues.  The pool provides
  a way for allocations to happen without them being tracked.  Collection is not
  automatic.  The pool must be cleared or destroyed for memory to be reclaimed.
  This affords the end user significant performance advantages in that each pool
  can be cleared independently.  Pools can be created per thread and at various
  scopes providing a mechanism to mostly (if not completely) eliminate memory
  fragmentation.  The pool object isn't thread safe.  In general, locks cause
  problems and if code can be designed to be thread safe without locking, it
  will perform better.  Many of the objects within the ac_ collection will use
  the pool for allocation for all of the reasons mentioned above.

  Clearing the pool generally consists of resetting a pointer.  Memory is only
  freed if the memory used by the pool exceeded the initial size assigned to it
  during initialization.  In this case, the extra blocks will be freed before
  the counter is reset.  It is normally best to set the initial size so that
  overflowing doesn't happen, except in rare circumstances.  The memory that was
  previously allocated prior to a clear will still possibly be valid, but
  shouldn't be relied upon.
*/

#ifndef _ac_pool_H
#define _ac_pool_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "another-c-library/ac_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ac_pool_s;
typedef struct ac_pool_s ac_pool_t;

/* ac_pool_init will create a working space of size bytes */
#ifdef _AC_MEMORY_CHECK_
#define ac_pool_init(size) _ac_pool_init(size, AC_FILE_LINE_MACRO("ac_pool"))
ac_pool_t *_ac_pool_init(size_t size, const char *caller);
#else
#define ac_pool_init(size) _ac_pool_init(size)
ac_pool_t *_ac_pool_init(size_t size);
#endif

/* ac_pool_pool_init creates a pool from another pool.  This can be useful for
   having a repeated clearing mechanism inside a larger pool.  Ideally, this
   pool should be sized right as the clear function can't free nodes. */
ac_pool_t *ac_pool_pool_init(ac_pool_t *pool, size_t initial_size);


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

/* duplicate all of the strings in a AND the NULL terminated pointer array.  */
char **ac_pool_strdupa(ac_pool_t *pool, char **a);

/* duplicate all of the strings in a AND the NULL terminated pointer array.  */
char **ac_pool_strdupan(ac_pool_t *pool, char **a, size_t num);

/* Duplicate the NULL terminated pointer array. */
char **ac_pool_strdupa2(ac_pool_t *pool, char **a);

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

/* split a string into N pieces using delimiter.  The array that is returned
   will always be valid with a NULL string at the end if p is NULL. num_splits
   can be NULL if the number of returning pieces is not desired. */
char **ac_pool_split(ac_pool_t *h, size_t *num_splits, char delim,
                     const char *p);

/* same as ac_split except allows formatting of input string. */
char **ac_pool_splitf(ac_pool_t *h, size_t *num_splits, char delim,
                      const char *p, ...);

/* same as ac_split except empty strings will not be included in the
   result. */
char **ac_pool_split2(ac_pool_t *h, size_t *num_splits, char delim,
                      const char *p);

/* same as ac_split2 except allows formatting of input string. */
char **ac_pool_split2f(ac_pool_t *h, size_t *num_splits, char delim,
                       const char *p, ...);


#include "another-c-library/ac-core/ac_pool.h"

#ifdef __cplusplus
}
#endif

#endif
