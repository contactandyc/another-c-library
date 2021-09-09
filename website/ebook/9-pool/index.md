---
path: "/9-pool"
posttype: "tutorial"
title: "9. The Pool Object"
---

What if our object could keep track of allocations and free memory for us?  This is generally the basis for languages that use garbage collection.  I'm going to illustrate how to build several objects which greatly reduce the number of calls to malloc and free (and reduce the risk that you will have memory leaks).  

The first is an object which I've used for almost my whole career.  I've called it a pool.  The basic idea with this object is that you can allocate and clear.  It has no free function.  When you clear, you clear all of the memory that has been previously allocated from the pool. Internally, it is very efficient because all it typically does is reset a counter to zero.  The base interface will look something like the following.

pool.h
```c
#ifndef _pool_H
#define _pool_H

struct pool_s;
typedef struct pool_s pool_t;

pool_t *pool_init();
void *pool_malloc(pool_t *h, size_t len);
void *pool_calloc(pool_t *h, size_t len);
char *pool_strdup(pool_t *h, char *p);
void pool_clear(pool_t *h);
void pool_destroy(pool_t *h);

#endif
```

There is no free method.  You can call the allocation methods as often as you like, and the allocations will remain valid until pool\_clear or pool\_destroy is called.

# The Simplest Implementation

When approaching algorithms, it is often a good idea to think about the simplest implementation.  This implementation doesn't need to be efficient.  It can serve to prove that a more complicated version achieves the same result.

Below describes a simple implementation of the pool interface:

- Have each allocation saved in a singly linked list on the pool\_s structure.  
- The clear method could free all of the memory in the linked list.
- The destroy could call clear, and then free the pool\_s object itself.
- Each allocation would require it's own link structure.

A singly linked list usually is linked to from a structure and then chained together using a member called next.  Each allocation would be placed in its own structure and linked to by the pool\_s structure through the member head.  The two objects and implementation could look like the following.

```c
typedef struct pool_node_s {
  void *m;
  struct pool_node_s *next;
} pool_node_t;

struct pool_s {
  pool_node_t *head;
};

pool_t *pool_init() {
  pool_t *r = (pool_t *)malloc(sizeof(pool_t));
  r->head = NULL;
  return r;
}

void *pool_malloc(pool_t *h, size_t len) {
  pool_node_t *n = (pool_node_t *)malloc(sizeof(pool_node_t));
  n->m = malloc(len);
  n->next = h->head;
  h->head = n;
  return n->m;
}

void *pool_calloc(pool_t *h, size_t len) {
  void *m = pool_malloc(h, len);
  memset(m, 0, len);
  return m;
}

char *pool_strdup(pool_t *h, char *p) {
  char *m = (char *)pool_malloc(h, strlen(p)+1);
  strcpy(m, p);
  return m;
}

void pool_clear(pool_t *h) {
  pool_node_t *n = h->head;
  while(n) {
    pool_node_t *next = n->next;
    free(n->m);
    free(n);
    n = next;
  }
}

void pool_destroy(pool_t *h) {
  pool_clear(h);
  free(h);
}
```

We could allocate the desired memory and the pool\_node\_t structure together.  If we did that, we wouldn't need the void *m parameter, and the memory requested could simply exist after the pool\_node\_t structure.

```c
typedef struct pool_node_s {
  struct pool_node_s *next;
} pool_node_t;


void *pool_malloc(pool_t *h, size_t len) {
  pool_node_t *n = (pool_node_t *)malloc(sizeof(pool_node_t) + len);
  n->next = h->head;
  h->head = n;
  return (void *)(n+1);
}

void pool_clear(pool_t *h) {
  pool_node_t *n = h->head;
  while(n) {
    pool_node_t *next = n->next;
    // free(n->m); // this is no longer needed
    free(n);
    n = next;
  }
}
```

Using the simple structure above, our allocations can be tracked and cleared by the pool object instead of requiring the application to remember where each allocation started from.


# A better implementation

The above implementation does save us from needing to free memory, but it doesn't do much more.  The pool object that exists in the src directory is listed below:

src/ac_pool.h
```c
#ifndef _ac_pool_H
#define _ac_pool_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "ac_allocator.h"

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

/* ac_pool_destroy frees up all memory associated with the pool object */
void ac_pool_destroy(ac_pool_t *h);

/* ac_pool_set_minimum_growth_size alters the minimum size of growth blocks.
   This is particularly useful if you don't expect the pool's block size to be
   exceeded by much, and you don't want the default, which would be to use the
   original block size for the new block (effectively doubling memory usage). */
void ac_pool_set_minimum_growth_size(ac_pool_t *h, size_t size);

/* ac_pool_alloc allocates len uninitialized bytes which are aligned. */
static inline void *ac_pool_alloc(ac_pool_t *h, size_t len);

/* ac_pool_alloc allocates len uninitialized bytes which are unaligned. */
static inline void *ac_pool_ualloc(ac_pool_t *h, size_t len);

/* ac_pool_alloc allocates len zero'd bytes which are aligned. */
static inline void *ac_pool_calloc(ac_pool_t *h, size_t len);

/* ac_pool_strdup allocates a copy of the string p.  The memory will be
  unaligned.  If you need the memory to be aligned, consider using ac_pool_dup
  like char *s = ac_pool_dup(pool, p, strlen(p)+1); */
static inline char *ac_pool_strdup(ac_pool_t *h, const char *p);

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
  overhead for the structures, and this is independent of any allocating calls.
*/
size_t ac_pool_used(ac_pool_t *h);

#include "impl/ac_pool.h"

#endif
```

A few significant changes are added to the interface.

- Everything uses the ac\_ prefix.
- The ac\_allocator object is used for allocation.
- ac\_pool\_init is changed to a macro to support memory debugging.
- ac\_pool\_init takes in a size (to support allocating in larger chunks)
- ac\_pool\_malloc is changed to ac\_pool\_alloc.
- ac\_pool\_ualloc supports unaligned allocation.
- ac\_pool\_dup and sla\_pool\_udup copy binary data.
- ac\_pool\_strdupf/ac\_pool\_strdupvf support working with format strings.
- ac\_pool\_size gets the overall number of bytes that have been allocated.
- ac\_pool\_used gets the overall number of bytes that have been allocated internally.
- impl/ac\_pool.h is used to inline a number of the functions for performance.
- some of the functions are declared as static inline because they are implemented in the impl/ac\_pool.h header file.

By this point, the code below should look pretty familiar.  Files are included, ac\_pool\_t is defined, and the \_ac\_pool\_H is defined to prevent the contents of this file from being included more than once.
```c
#ifndef _ac_pool_H
#define _ac_pool_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "ac_allocator.h"

struct ac_pool_s;
typedef struct ac_pool_s ac_pool_t;
```

Because this object will use the ac\_allocator for debugging memory, the following is needed to define the init function.
```c
/* ac_pool_init will create a working space of size bytes */
#ifdef _AC_DEBUG_MEMORY_
#define ac_pool_init(size) _ac_pool_init(size, AC_FILE_LINE_MACRO("ac_pool"))
ac_pool_t *_ac_pool_init(size_t size, const char *caller);
#else
#define ac_pool_init(size) _ac_pool_init(size)
ac_pool_t *_ac_pool_init(size_t size);
#endif
```

If we weren't using the ac\_allocator, our init call would look like this:
```c
ac_pool_t *_ac_pool_init(size_t size);
```

If you refer back to the end of the [allocator](../7-allocator/index.md) code, there is an explanation of similar code for the timer object conversion.

Replace
```c
ac_pool_t *ac_pool_init(size_t size);
```

with
```c
#ifdef _AC_DEBUG_MEMORY_
#define ac_pool_init(size) _ac_pool_init(size, AC_FILE_LINE_MACRO("ac_pool"))
ac_pool_t *_ac_pool_init(size_t size, const char *caller);
#else
#define ac_pool_init(size) _ac_pool_init(size)
ac_pool_t *_ac_pool_init(size_t size);
#endif
```

The above code has two basic cases: one where \_AC\_DEBUG\_MEMORY\_ is defined, and the other where it is not (#else).  It may be easier to break this into a couple of steps.

1.  Convert the init function to be prefixed with an underscore

```c
ac_pool_t *ac_pool_init(size_t size);
```

becomes
```c
ac_pool_t *_ac_pool_init(size_t size);
```

2.  Create a macro which defines ac\_pool\_init as \_ac\_pool\_init
```c
#define ac_pool_init(size) _ac_pool_init(size)
ac_pool_t *_ac_pool_init(size_t size);
```

3.  Define the macro `if` logic with the `else` part filled in.
```c
#ifdef _AC_DEBUG_MEMORY_
#else
#define ac_pool_init(size) _ac_pool_init(size)
ac_pool_t *_ac_pool_init(size_t size);
#endif
```

4.  Add const char *caller to the debug version of \_ac\_pool\_init
```c
ac_pool_t *_ac_pool_init(size_t size, const char *caller);
```

5.  Define the macro to call the init function.
```c
#define ac_pool_init(size) _ac_pool_init(size, AC_FILE_LINE_MACRO("ac_pool"))
```

6.  Put the two calls in the #ifdef \_AC\_DEBUG\_MEMORY\_ section.
```c
#ifdef _AC_DEBUG_MEMORY_
#define ac_pool_init(size) _ac_pool_init(size, AC_FILE_LINE_MACRO("ac_pool"))
ac_pool_t *_ac_pool_init(size_t size, const char *caller);
#else
#define ac_pool_init(size) _ac_pool_init(size)
ac_pool_t *_ac_pool_init(size_t size);
#endif
```

A common use case for the pool is to allocate a bunch of times and then clear and repeat.  For example, if you were responding to queries from a search engine, you might want to use this object and allocate a bunch of times for parsing the query and building a response.  Once the response has been written, the pool can be cleared and used for the next query.  The pool would likely need a certain amount of bytes to satisfy the vast majority of the queries.  Given this use, it makes sense for the pool to allocate enough bytes in a single block, and then dole out the memory via its allocation methods.  If the single block isn't big enough for all of the allocations, overflow blocks will be allocated.  When the pool is cleared, the overflow blocks will be freed, the main block will remain, and the counter inside the pool will be reset to zero.  If we make the pool too small, overflow blocks will have to be constantly allocated.  If we make it too big, we waste memory.

# TO BE CONTINUED

[Table of Contents (only if viewing on Github)](../../../README.md)
