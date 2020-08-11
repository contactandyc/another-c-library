---
title: ac_pool
description:
---

```c
#include "ac_pool.h"
```

The ac_pool provides an api similar to malloc, calloc, strdup, along with many other useful common allocation patterns with the exception of free. The pool must be cleared (ac_pool_clear) or destroyed (ac_pool_destroy) to reclaim  memory allocated.

C doesn't have garbage collection. Many C developers prefer to stay away from languages which have it as it can cause performance issues. The pool provides a way for allocations to happen without them being tracked. Collection is not automatic. The pool must be cleared or destroyed for memory to be reclaimed. This affords the end user significant performance advantages in that each pool can be cleared independently. Pools can be created per thread and at various scopes providing a mechanism to mostly (if not completely) eliminate memory fragmentation. The pool object isn't thread safe. In general, locks cause problems and if code can be designed to be thread safe without locking, it will perform better. Many of the objects within the ac_ collection will use the pool for allocation for all of the reasons mentioned above.

Clearing the pool generally consists of resetting a pointer. Memory is only freed if the memory used by the pool exceeded the initial size assigned to it during initialization. In this case, the extra blocks will be freed before the counter is reset. It is normally best to set the initial size so that overflowing doesn't happen, except in rare circumstances. The memory that was previously allocated prior to a clear will still possibly be valid, but shouldn't be relied upon.

The following files are necessary to include in your own package. You can also just include the whole ac_ library.

```
ac_allocator.h
ac_allocator.c
ac_pool.h
ac_pool.c
impl/ac_pool.h
ac_common.h
```

All of the example code is found in examples/ac_pool. 

## ac_pool_init

```c
ac_pool_t *ac_pool_init(size_t size);
```

ac_pool_init initializes a pool object of size bytes. The pool internally allocates a block of ram size bytes long and then gives pieces of it out upon allocation requests. If more memory is needed, the pool will grow. It is generally recommended to size it properly, unless your use case doesn't include clearing the pool.

```c
void ac_pool_set_minimum_growth_size(ac_pool_t *h, size_t size);
```
ac_pool_set_minimum_growth_size alters the minimum size of growth blocks. This is particularly useful if you don't expect the pool's block size to be exceeded by much and you don't want the default which would be to use the original block size for the new block (effectively doubling memory usage).

## ac_pool_clear

```c
void ac_pool_clear(ac_pool_t *h);
```
ac_pool_clear will make all of the pool's memory reusable. If the initial block was exceeded and additional blocks were added, those blocks will be freed.

## ac_pool_destroy
```c
void ac_pool_destroy(ac_pool_t *h);
```
ac_pool_destroy destroys the pool. Calling ac_pool_destroy will invalidate all of the memory that has been allocated from the pool object.

## ac_pool_alloc

```c
void *ac_pool_alloc(ac_pool_t *h, size_t len);
```
ac_pool_alloc allocates len uninitialized bytes which are aligned.

## ac_pool_ualloc

```c
void *ac_pool_ualloc(ac_pool_t *h, size_t len);
```
ac_pool_ualloc allocates len uninitialized bytes which are unaligned.

## ac_pool_calloc

```c
void *ac_pool_calloc(ac_pool_t *h, size_t len);
```
ac_pool_calloc allocates len zero'd bytes which are aligned.

## ac_pool_strdup

```c
char *ac_pool_strdup(ac_pool_t *h, const char *p);
```
ac_pool_strdup allocates a copy of the string p. The memory will be unaligned. If you need the memory to be aligned, consider using ac_pool_dup and specifying the string length + 1 for the zero terminator.

## ac_pool_strndup
```c
char *ac_pool_strndup(ac_pool_t *h, const char *p, size_t length);
```
ac_pool_strndup is similar to ac_pool_strdup, except the returned string is limited to length (+1 for zero terminator) bytes.

## ac_pool_strdupf

```c
char *ac_pool_strdupf(ac_pool_t *h, const char *p, ...);
```
ac_pool_strdupf allocates a copy of the formatted string p.


## ac_pool_strdupvf

```c
char *ac_pool_strdupvf(ac_pool_t *h, const char *format, va_list args);
```
ac_pool_strdupvf is similar to ac_pool_strdupf, except that it uses va_list args. For example, ac_pool_strdupf is implemented using this method as follows.

```c
char *ac_pool_strdupf(ac_pool_t *pool, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char *r = ac_pool_strdupvf(pool, fmt, args);
  va_end(args);
  return r;
}
```

You can implement your own strdupf like functions in a similar manner for other objects that rely upon the pool for internal memory.

## ac_pool_dup

```c
void *ac_pool_dup(ac_pool_t *h, const void *data, size_t len);
```
ac_pool_dup allocates a copy of the data. The memory will be aligned.

## ac_pool_udup

```c
void *ac_pool_udup(ac_pool_t *h, const void *data, size_t len);
```
ac_pool_udup allocates a copy of the data. The memory will be unaligned.

## ac_pool_strdupa

```c
char **ac_pool_strdupa(ac_pool_t *pool, char **a);
```
ac_pool_strdupa duplicates all of the strings in the array **a** AND the NULL terminated pointer array.

## ac_pool_strdupan

```c
char **ac_pool_strdupan(ac_pool_t *pool, char **a, size_t num);
```
ac_pool_strdupan duplicates all of the strings in the array **a** AND the pointer array which has num elements in it. The duplicated copy will also be NULL terminated.

## ac_pool_strdupa2

```c
char **ac_pool_strdupa2(ac_pool_t *pool, char **a);
```
ac_pool_strdupa2 duplicates the NULL terminated pointer array. All of the referenced strings in the array **a** are referenced in the returned result.

## ac_pool_tokenize

```c
char **ac_pool_tokenize(ac_pool_t *h, size_t *num_splits, const char *delim,
                        const char *p);
```
ac_pool_tokenize splits a string into N pieces using any character in delim array. The array that is returned will always be valid with a NULL string at the end if p is NULL. num_splits can be NULL if the number of returning pieces is not desired.

## ac_pool_split

```c
char **ac_pool_split(ac_pool_t *h, size_t *num_splits, char delim,
                     const char *p);
```
ac_pool_split splits a string into N pieces using delimiter. The array that is returned will always be valid with a NULL string at the end if p is NULL. num_splits can be NULL if the number of returning pieces is not desired.

## ac_pool_splitf

```c
char **ac_pool_splitf(ac_pool_t *h, size_t *num_splits, char delim,
                      const char *p, ...);
```
ac_pool_splitf is similar as ac_pool_split except allows formatting of input string.

## ac_pool_split2

```c
char **ac_pool_split2(ac_pool_t *h, size_t *num_splits, char delim,
                      const char *p);
```
ac_pool_split2 is similar as ac_pool_split except empty strings will not be included in the result.

```c
char **ac_pool_split2f(ac_pool_t *h, size_t *num_splits, char delim,
                       const char *p, ...);
```
ac_pool_split2f is similar as ac_pool_split2 except allows formatting of input string.

## ac_pool_size

```c
size_t ac_pool_size(ac_pool_t *h);
```
ac_pool_size returns the number of bytes that have been allocated from any of the alloc calls above (after initialization or the most recent clear).

## ac_pool_used

```c
size_t ac_pool_used(ac_pool_t *h);
```
ac_pool_used returns the number of bytes that have been allocated by the pool itself. This will always be greater than ac_pool_size as there is overhead for the structures and this is independent of any allocating calls.

## ac_pool_min_max_alloc

```c
void *ac_pool_min_max_alloc(ac_pool_t *h, size_t *rlen,
                            size_t min_len, size_t len);
```
ac_pool_min_max_alloc allocates up to len bytes, but not less than min_len bytes. Internally, the pool uses large blocks of memory which are often partially used up. min_max_alloc will only cause the pool to grow if there isn't min_len bytes available.

## ac_pool_checkpoint
## ac_pool_reset

```c
void ac_pool_checkpoint(ac_pool_t *h, ac_pool_checkpoint_t *cp);
void ac_pool_reset(ac_pool_t *h, ac_pool_checkpoint_t *cp);
```

ac_pool_checkpoint and ac_pool_reset work together. ac_pool_checkpoint sets a marker in the pool for which it can be reset to with ac_pool_reset. This can be useful if you want to allow the pool to grow and shrink back to a certain point.

<NextPrev prev="ac_out" prevUrl="/docs/ac-out" next="ac_sort" nextUrl="/docs/ac-sort" />
