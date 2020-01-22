---
path: "/ac-pool"
posttype: "docs"
title: "ac_pool"
---


# ac_pool

The ac_pool provides an api similar to malloc, calloc, strdup, along with many other useful common allocation patterns.  The pool uses an allocate and clear approach.  There is no free method.  The clear call clears all of the memory that has been previously allocated from the pool. Internally, a counter is to zero to clear and the counter is incremented to allocate (most of the time), so it is very efficient.  The pool should be initialized with a size such that
it doesn't need to grow most of the time prior to clearing.  There can be a slight advantage to using a size that is a multiple of 4096 when selecting a size (do this instead of selecting 3800 or something close to 4096).

### Commonly used functions
```c
#include "ac_pool.h"

/* initialize a pool object of size bytes */
ac_pool_t *ac_pool_init(size_t size);

/* ac_pool_set_minimum_growth_size alters the minimum size of growth blocks.
   This is particularly useful if you don't expect the pool's block size to be
   exceeded by much and you don't want the default which would be to use the
   original block size for the new block (effectively doubling memory usage). */
void ac_pool_set_minimum_growth_size(ac_pool_t *h, size_t size);

/* ac_pool_alloc allocates len uninitialized bytes which are aligned. */
void *ac_pool_alloc(ac_pool_t *h, size_t len);

/* ac_pool_alloc allocates len zero'd bytes which are aligned. */
void *ac_pool_calloc(ac_pool_t *h, size_t len);

/* ac_pool_strdup allocates a copy of the string p.  The memory will be
  unaligned.  If you need the memory to be aligned, consider using ac_pool_dup
  like char *s = ac_pool_dup(pool, p, strlen(p)+1); */
char *ac_pool_strdup(ac_pool_t *h, const char *p);

/* ac_pool_strdupf allocates a copy of the formatted string p. */
char *ac_pool_strdupf(ac_pool_t *h, const char *p, ...);

/* ac_pool_dup allocates a copy of the data.  The memory will be aligned. */
void *ac_pool_dup(ac_pool_t *h, const void *data, size_t len);

/* duplicate all of the strings in a AND the NULL terminated pointer array.  */
char **ac_pool_strdupa(ac_pool_t *pool, char **a);

/* duplicate all of the strings in a AND the NULL terminated pointer array.  */
char **ac_pool_strdupan(ac_pool_t *pool, char **a, size_t num);

/* Duplicate the NULL terminated pointer array. */
char **ac_pool_strdupa2(ac_pool_t *pool, char **a);

/* split a string into N pieces using any character in delim array.  The array
   that is returned will always be valid with a NULL string at the end if p is
   NULL. num_splits can be NULL if the number of returning pieces is not
   desired. */
char **ac_pool_tokenize(ac_pool_t *h, size_t *num_splits, const char *delim,
                        const char *p);

/* split a string into N pieces using delimiter.  The array that is returned
   will always be valid with a NULL string at the end if p is NULL. num_splits
   can be NULL if the number of returning pieces is not desired. */
char **ac_pool_split(ac_pool_t *h, size_t *num_splits, char delim,
                     const char *p);

/* same as ac_pool_split except allows formatting of input string. */
char **ac_pool_splitf(ac_pool_t *h, size_t *num_splits, char delim,
                      const char *p, ...);

/* same as ac_pool_split except empty strings will not be included in the
   result. */
char **ac_pool_split2(ac_pool_t *h, size_t *num_splits, char delim,
                      const char *p);

/* same as ac_pool_split2 except allows formatting of input string. */
char **ac_pool_split2f(ac_pool_t *h, size_t *num_splits, char delim,
                       const char *p, ...);

/* like ac_pool_strdup, limited to length (+1 for zero terminator) bytes */
static inline char *ac_pool_strndup(ac_pool_t *h, const char *p, size_t length);


/* ac_pool_size returns the number of bytes that have been allocated from any
  of the alloc calls above.  */
size_t ac_pool_size(ac_pool_t *h);

/* ac_pool_used returns the number of bytes that have been allocated by the
  pool itself.  This will always be greater than ac_pool_size as there is
  overhead for the structures and this is independent of any allocating calls.
*/
size_t ac_pool_used(ac_pool_t *h);

/* ac_pool_clear will make all of the pool's memory reusable.  If the
  initial block was exceeded and additional blocks were added, those blocks
  will be freed. */
void ac_pool_clear(ac_pool_t *h);

/* destroy the pool.  Calling ac_pool_destroy will invalidate all of the
   memory that has been allocated from the pool object. */
void ac_pool_destroy(ac_pool_t *h);
```

### More advanced functions or less used
```c
/* ac_pool_strdupvf similar to ac_pool_strdupf, except that it uses va_list
   args.  For example, ac_pool_strdupf is implemented using this method as
   follows.

      char *ac_pool_strdupf(ac_pool_t *pool, const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        char *r = ac_pool_strdupvf(pool, fmt, args);
        va_end(args);
        return r;
      }

   You can implement your own strdupf like functions in a similar manner for
   other objects that rely upon the pool for internal memory. */
char *ac_pool_strdupvf(ac_pool_t *h, const char *format, va_list args);

/* ac_pool_min_max_alloc allocates up to len bytes, but not less than
   min_len bytes.  Internally, the pool uses large blocks of memory which
   are often partially used up.  min_max_alloc will only cause the pool to
   grow if there isn't min_len bytes available. */
void *ac_pool_min_max_alloc(ac_pool_t *h, size_t *rlen,
                            size_t min_len, size_t len);

/* ac_pool_alloc allocates len uninitialized bytes which are unaligned. */
void *ac_pool_ualloc(ac_pool_t *h, size_t len);

/* ac_pool_dup allocates a copy of the data.  The memory will be unaligned. */
void *ac_pool_udup(ac_pool_t *h, const void *data, size_t len);

/* Checkpoint and reset work together.  ac_pool_checkpoint sets a marker in
   the pool for which it can be reset to with ac_pool_reset.  This can be
   useful if you want to allow the pool to grow and shrink back to a certain
   point. */
void ac_pool_checkpoint(ac_pool_t *h, ac_pool_checkpoint_t *cp);
void ac_pool_reset(ac_pool_t *h, ac_pool_checkpoint_t *cp);
```

## Dependencies
Dependencies are the files necessary to include in your own package.  You can also just include the whole ac_ library.
```
ac_allocator.h
ac_allocator.c
ac_pool.h
ac_pool.c
ac_common.h
```
