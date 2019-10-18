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

#ifndef _acpool_H
#define _acpool_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "acallocator.h"

struct acpool_s;
typedef struct acpool_s acpool_t;

/* acpool_init will create a working space of size bytes */
#ifdef _ACDEBUG_MEMORY_
#define acpool_init(size)                                                   \
  _acpool_init(size, ACFILE_LINE_MACRO("acpool"))
acpool_t *_acpool_init(size_t size, const char *caller);
#else
#define acpool_init(size) _acpool_init(size)
acpool_t *_acpool_init(size_t size);
#endif

/* acpool_clear will make all of the pool's memory reusable.  If the
  initial block was exceeded and additional blocks were added, those blocks
  will be freed. */
void acpool_clear(acpool_t *h);

/*
acpool_checkpoint_t *acget_checkpoint(acpool_t *pool);

void acpool_clear_to(acpool_t *h, acpool_checkpoint_t *cp);
*/

/* acpool_destroy frees up all memory associated with the pool object */
void acpool_destroy(acpool_t *h);

/* acpool_set_minimum_growth_size alters the minimum size of growth blocks.
   This is particularly useful if you don't expect the pool's block size to be
   exceeded by much and you don't want the default which would be to use the
   original block size for the new block (effectively doubling memory usage). */
void acpool_set_minimum_growth_size(acpool_t *h, size_t size);

/* acpool_alloc allocates len uninitialized bytes which are aligned. */
static inline void *acpool_alloc(acpool_t *h, size_t len);

/* acpool_alloc allocates len uninitialized bytes which are unaligned. */
static inline void *acpool_ualloc(acpool_t *h, size_t len);

/* acpool_alloc allocates len zero'd bytes which are aligned. */
static inline void *acpool_calloc(acpool_t *h, size_t len);

/* acpool_strdup allocates a copy of the string p.  The memory will be
  unaligned.  If you need the memory to be aligned, consider using acpool_dup
  like char *s = acpool_dup(pool, p, strlen(p)+1); */
static inline char *acpool_strdup(acpool_t *h, const char *p);

/* like acpool_strdup, limited to length (+1 for zero terminator) bytes */
static inline char *acpool_strndup(acpool_t *h, const char *p,
                                      size_t length);

/* acpool_dup allocates a copy of the data.  The memory will be aligned. */
static inline void *acpool_dup(acpool_t *h, const void *data, size_t len);

/* acpool_dup allocates a copy of the data.  The memory will be unaligned. */
static inline void *acpool_udup(acpool_t *h, const void *data,
                                   size_t len);

/* acpool_strdupf allocates a copy of the formatted string p. */
static inline char *acpool_strdupf(acpool_t *h, const char *p, ...);

/* acpool_strdupvf allocates a copy of the formatted string p. This is
  particularly useful if you wish to extend another object which uses pool as
  its base.  */
char *acpool_strdupvf(acpool_t *h, const char *p, va_list args);

/* acpool_size returns the number of bytes that have been allocated from any
  of the alloc calls above.  */
size_t acpool_size(acpool_t *h);

/* acpool_used returns the number of bytes that have been allocated by the
  pool itself.  This will always be greater than acpool_size as there is
  overhead for the structures and this is independent of any allocating calls.
*/
size_t acpool_used(acpool_t *h);

#include "impl/acpool.h"

#endif
