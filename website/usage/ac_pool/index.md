# ac_pool

The ac_pool provides an api similar to malloc, calloc, strdup, and more.  The pool uses an allocate and clear approach.  There is no free method.  It has no free function. The clear call clears all of the memory that has been previously allocated from the pool. Internally, resets a counter to zero to clear and adds to the counter to allocate (most of the time), so it is very efficient.


### Commonly used functions
```c
#include "ac_pool.h"

ac_pool_t *ac_pool_init(size_t size);
void ac_pool_set_minimum_growth_size(ac_pool_t *h, size_t size);

void *ac_pool_alloc(ac_pool_t *h, size_t len);
void *ac_pool_calloc(ac_pool_t *h, size_t len);
char *ac_pool_strdup(ac_pool_t *h, const char *p);
char *ac_pool_strdupf(ac_pool_t *h, const char *p, ...);
void *ac_pool_dup(ac_pool_t *h, const void *data, size_t len);

size_t ac_pool_size(ac_pool_t *h);
size_t ac_pool_used(ac_pool_t *h);

void ac_pool_clear(ac_pool_t *h);

void ac_pool_destroy(ac_pool_t *h);
```

The pool is initialized with a size which will become the internal normal usage.  The pool will grow by a minimum growth size (or more) if needed which can be set after initialization.  The default minimum growth size is the size that is passed in.  There can be a slight advantage to using a size that is a multiple of 4096 when selecting a size (do this instead of selecting 3800 or something close to 4096).

The pool has a number of allocation methods.  To keep things simpler, I've listed the ones which are the easiest to understand first.

void \*ac_pool_alloc(ac_pool_t \*h, size_t length);<br/>
Returns a pointer to length uninitialized bytes (similar to malloc).

void \*ac_pool_calloc(ac_pool_t \*h, size_t length);<br/>
Returns a pointer to length zeroed bytes (similar to calloc).

char \*ac_pool_strdup(ac_pool_t \*h, const char \*p);<br/>
Returns a pointer to a copy of the string passed into it (similar to strdup).

char \*ac_pool_strdupf(ac_pool_t \*h, const char \*p, ...);<br/>
Returns a pointer to a copy of the string passed into it (similar to strdup).

void \*ac_pool_dup(ac_pool_t \*, const void \*data, size_t length);<br/>
Returns a pointer to a copy of data (which is length bytes).

size_t ac_pool_size(ac_pool_t *h);<br/>
Returns the number of bytes that have been allocated from any of the alloc calls above.

size_t ac_pool_used(ac_pool_t *h);<br/>
Returns the number of bytes that have been allocated by the pool itself.  This will always be greater than ac_pool_size as there is overhead for the structures and this is independent of any allocating calls.

void ac_pool_clear(ac_pool_t *h);<br/>
The pool is meant to be allocated from a number of times and then cleared.  ac_pool_clear invalidates all of the memory that has been allocated previously.  This function is very efficient and can be called even if no prior allocations have happened.

void ac_pool_destroy(ac_pool_t *h);<br/>
Once the pool is no longer needed, call ac_pool_destroy to clean up all of its resources.  Calling ac_pool_destroy will invalidate all of the memory that has been allocated from the pool object.


### More advanced functions or less used
```c
char *ac_pool_strdupvf(ac_pool_t *h, const char *format, va_list args);
void *ac_pool_min_max_alloc(ac_pool_t *h, size_t *rlen,
                            size_t min_len, size_t len);
void *ac_pool_ualloc(ac_pool_t *h, size_t len);
void *ac_pool_udup(ac_pool_t *h, const void *data, size_t len);

void ac_pool_checkpoint(ac_pool_t *h, ac_pool_checkpoint_t *cp);
void ac_pool_reset(ac_pool_t *h, ac_pool_checkpoint_t *cp);
```

char \*ac_pool_strdupvf(ac_pool_t \*h, const char \*format, va_list args);<br/>
Similar to ac_pool_strdupf, except that it uses va_list args.  ac_pool_strdupf is implemented using this method as follows.

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

void *ac_pool_min_max_alloc(ac_pool_t *h, size_t *rlen, size_t min_len, size_t len);<br/>
Allocates up to len bytes, but not less than min_len bytes.  Internally, the pool uses large blocks of memory which are often partially used up.  min_max_alloc will only cause the pool to grow if there isn't min_len bytes available.

void *ac_pool_ualloc(ac_pool_t *h, size_t len);<br/>
ac_pool_alloc and ac_pool_calloc both returned memory that is aligned.  This will return memory that is potentially unaligned (it is used internally by strdup... methods).

void *ac_pool_udup(ac_pool_t *h, const void *data, size_t len);<br/>
ac_pool_dup returns memory that is aligned.  This will potentially return memory that is unaligned.

void ac_pool_checkpoint(ac_pool_t *h, ac_pool_checkpoint_t *cp);<br/>
void ac_pool_reset(ac_pool_t *h, ac_pool_checkpoint_t *cp);<br/>
Checkpoint and reset work together.  ac_pool_checkpoint sets a marker in the pool for which it can be reset to with ac_pool_reset.  This can be useful if you want to allow the pool to grow and shrink back to a certain point.

## Dependencies
Dependencies are the files necessary to include in your own package.  You can also just include the whole ac_ library.
```
ac_allocator.h
ac_allocator.c
ac_pool.h
ac_pool.c
ac_common.h
```
