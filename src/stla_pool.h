#ifndef _stla_pool_H
#define _stla_pool_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

struct stla_pool_s;
typedef struct stla_pool_s stla_pool_t;

/* For functions which begin with _, call them without the underscore.  They
  are meant to be called through a macro. */

/* stla_pool_init will create a working space of size bytes */
stla_pool_t *_stla_pool_init(size_t size);

/* stla_pool_clear will make all of the pool's memory reusable.  If the
  initial block was exceeded and additional blocks were added, those blocks
  will be freed. */
void _stla_pool_clear(stla_pool_t *h);

/* stla_pool_destroy frees up all memory associated with the pool object */
void _stla_pool_destroy(stla_pool_t *h);

/* stla_pool_set_minimum_growth_size alters the minimum size of growth blocks.
   This is particularly useful if you don't expect the pool's block size to be
   exceeded by much and you don't want the default which would be to use the
   original block size for the new block (effectively doubling memory usage). */
void stla_pool_set_minimum_growth_size(stla_pool_t *h, size_t size);

/* stla_pool_alloc allocates len uninitialized bytes which are aligned. */
static inline void *stla_pool_alloc(stla_pool_t *h, size_t len);

/* stla_pool_alloc allocates len uninitialized bytes which are unaligned. */
static inline void *stla_pool_ualloc(stla_pool_t *h, size_t len);

/* stla_pool_alloc allocates len zero'd bytes which are aligned. */
static inline void *stla_pool_calloc(stla_pool_t *h, size_t len);

/* stla_pool_strdup allocates a copy of the string p.  The memory will be
  unaligned.  If you need the memory to be aligned, consider using stla_pool_dup
  like char *s = stla_pool_dup(pool, p, strlen(p)+1); */
static inline char *stla_pool_strdup(stla_pool_t *h, const char *p);

/* stla_pool_dup allocates a copy of the data.  The memory will be aligned. */
static inline char *stla_pool_dup(stla_pool_t *h, const void *data, size_t len);

/* stla_pool_strdupf allocates a copy of the formatted string p. */
static inline char *stla_pool_strdupf(stla_pool_t *h, const char *p, ...);

/* stla_pool_strdupvf allocates a copy of the formatted string p. This is
  particularly useful if you wish to extend another object which uses pool as
  its base.  */
char *stla_pool_strdupvf(stla_pool_t *h, const char *p, va_list args);

/* stla_pool_size returns the number of bytes that have been allocated from any
  of the alloc calls above.  */
size_t stla_pool_size(stla_pool_t *h);

/* stla_pool_used returns the number of bytes that have been allocated by the
  pool itself.  This will always be greater than stla_pool_size as there is
  overhead for the structures and this is independent of any allocating calls.
*/
size_t stla_pool_used(stla_pool_t *h);

#include "impl/stla_pool.h"

#endif
