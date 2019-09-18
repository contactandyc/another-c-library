# Pool

A quick note about C allocation methods...

In C, there are five basic functions for allocating memory (technically there are more, but they are rarely used).
```c
void *malloc(size_t len);
void *calloc(size_t num, size_t len);
void *realloc(void *p, size_t len);
char *strdup(char *p);
void free(void *p);
```

The middle three functions essentially work as follows (although the internal implementation is more complex for performance reasons).
```c
void *calloc(size_t num, size_t len) {
  void *res = malloc(num*len);
  memset(res, 0, num*len);
  return res;
}

void *realloc(void *p, size_t len) {
  size_t old_len = extract_length(p); // details of this function depend upon how malloc is implemented
  void *res = malloc(len);
  if(len < old_len)
    memcpy(res, p, len);
  else
    memcpy(res, p, old_len);
  free(p);
  return res;
}

char *strdup(char *p) {
  size_t len = strlen(p)+1;
  char *res = (char *)malloc(len);
  memcpy(res, p, len);
  return res;
}
```

It's important to realize that malloc (memory alloc) is the core behind all of these functions or at a minimum, they are derivates of malloc.

One of the reasons that people tend to steer clear of C is because you must maintain pointers to memory that you allocate so that it can later be freed.  In the last chapter, I introduced doubly linked lists.  We can implement a way of tracking allocations using a doubly linked list.  The functions that we will implement are the ones defined above - so the interface will be pretty straight forward.  To prevent naming conflicts, we will use the stla prefix.

```c
#ifndef _stla_global_allocator_H
#define _stla_global_allocator_H

#include <stdlib.h> /* for size_t */

void *stla_malloc(size_t len);
void *stla_calloc(size_t num, size_t block_size);
void *stla_realloc(void *p, size_t len);
char *stla_strdup(char *p);
void stla_free(void *p);

#endif
```

This is what the API should look like, but we can do a little better.  These functions will all be called from a line of code which will be in a file.  The c compiler allows us to create a macro (basically defining one thing as another).  There are three special constants that can be useful for debugging.

```
__LINE__ - The line of code that you are on.
__FILE__ - The file that the code exists in.
__FUNCTION__ - The function that the code exists in.
```

For example if you were to create this
test_special_constants.c
```c
#include <stdio.h>

int main( int argc, char *argv[]) {
  printf( "This line of code is at line %d in function %s in the file %s\n", __LINE__, __FUNCTION__, __FILE__);
  return 0;
}
```

Build it
```bash
gcc test_special_constants.c -o test_special_constants
```

And run it
```bash
$ ./test_special_constants
This line of code is at line 4 in function main in the file test_special_constants.c
```

__FILE__ and __FUNCTION__ are of the type const char * (meaning they are a sequence of read only characters).   __LINE__ is an int (integer).

We could redefine our interface as

```c
#ifndef _stla_global_allocator_H
#define _stla_global_allocator_H

#include <stdlib.h> /* for size_t */

void *stla_malloc(const char *filename, int line, size_t len);
void *stla_calloc(const char *filename, int line, size_t num, size_t block_size);
void *stla_realloc(const char *filename, int line, void *p, size_t len);
char *stla_strdup(const char *filename, int line, char *p);
void stla_free(const char *filename, void *p);

#endif
```

and then call our functions like this.

sample.c (this won't work - because I'm not building the object this way in the end)
```c
#include "stla_global_allocator.h"

int main( int argc, char *argv[] ) {
  char *s = (char *)stla_malloc(__FILE__, __LINE__, 100);
  stla_free(__FILE__, __LINE__, s);
  return 0;
}
```

There is a pattern above in that everytime we call stla_malloc (and others), we would call it with __FILE__, __LINE__ as the first two parameters.  We can utilize the compiler and macros to help us here.  In C, you define a macro using #define.  

For example,
```c
#include <stdio.h>

#define multiply(x, y) x*y

int main( int argc, char *argv[]) {
  printf("5 x 6 = %d\n", multiply(5, 6));
  return 0;
}
```

gets interpreted as
```c
#include <stdio.h>

#define multiply(x, y) x*y

int main( int argc, char *argv[]) {
  printf("5 x 6 = %d\n", 5*6);
  return 0;
}
```

Before it is turned into a binary.  The macro literally gets placed inline in the code.  For example, a macro can reference a variable that was defined in the function.

```c
#include <stdio.h>

#define multiply_by_x(y) x*y

int main( int argc, char *argv[]) {
  int x = 5;
  printf("5 x 6 = %d\n", multiply_by_x(6));
  return 0;
}
```

Notice that x doesn't exist when multiply_by_x is defined.  Because it is literally replaced it would look like the following.

```c
#include <stdio.h>

#define multiply_by_x(y) x*y

int main( int argc, char *argv[]) {
  int x = 5;
  printf("5 x 6 = %d\n", x*6);
  return 0;
}
```

If x didn't exist, the compiler would ultimately throw an error.

```c
#include <stdio.h>

#define multiply_by_x(y) multiply(x, y)

int multiply(int x, int y) {
  return x*y;
}

int main( int argc, char *argv[]) {
  int x = 5;
  printf("5 x 6 = %d\n", multiply_by_x(6));
  return 0;
}
```

get's converted to

```c
#include <stdio.h>

#define multiply_by_x(y) multiply(x, y)

int multiply(int x, int y) {
  return x*y;
}

int main( int argc, char *argv[]) {
  int x = 5;
  printf("5 x 6 = %d\n", multiply(x, 6));
  return 0;
}
```

Notice that multiply didn't have to exist when the macro was defined.  Macros are evaluated first.  

Macros almost never end in a semicolon.  The following will throw an error.

test_code.c
```c
#include <stdio.h>

#define multiply_by_x(y) multiply(x, y);

int multiply(int x, int y) {
  return x*y;
}

int main( int argc, char *argv[]) {
  int x = 5;
  printf("5 x 6 = %d\n", multiply_by_x(6));
  return 0;
}
```

```bash
$ gcc test_code.c -o test_code
test_special_constants.c:11:26: error: unexpected ';' before ')'
  printf("5 x 6 = %d\n", multiply_by_x(6));
                         ^
test_special_constants.c:3:40: note: expanded from macro 'multiply_by_x'
#define multiply_by_x(y) multiply(x, y);
                                       ^
```

The above code get's converted to
```c
#include <stdio.h>

#define multiply_by_x(y) multiply(x, y);

int multiply(int x, int y) {
  return x*y;
}

int main( int argc, char *argv[]) {
  int x = 5;
  printf("5 x 6 = %d\n", multiply(x, 6););
  return 0;
}
```

Notice the extra semicolon after multiply.  In general, macros can have semicolons in them, but they cannot end in a semicolon.  Macros can also define multiple lines of code (or multiple statements).

test_code.c
```c
#include <stdio.h>

#define swap(a,b) \
  tmp = a;        \
  a = b;          \
  b = tmp

int main( int argc, char *argv[] ) {
  int x = 5;
  int y = 10;
  printf( "before swap: (%d, %d)\n", x, y );
  swap(x, y);
  printf( "after swap: (%d, %d)\n", x, y );
  return 0;
}
```

```bash
$ gcc test_code.c -o test_code
test_special_constants.c:12:3: error: use of undeclared identifier 'tmp'
  swap(x, y);
  ^
test_special_constants.c:4:3: note: expanded from macro 'swap'
  tmp = a;        \
  ^
test_special_constants.c:12:3: error: use of undeclared identifier 'tmp'
test_special_constants.c:6:7: note: expanded from macro 'swap'
  b = tmp
      ^
2 errors generated.
```

The swap macro expected a tmp variable to exist.  Declaring tmp will fix the code.

test_code.c
```c
#include <stdio.h>

#define swap(a,b) \
  tmp = a;        \
  a = b;          \
  b = tmp

int main( int argc, char *argv[] ) {
  int tmp;
  int x = 5;
  int y = 10;
  printf( "before swap: (%d, %d)\n", x, y );
  swap(x, y);
  printf( "after swap: (%d, %d)\n", x, y );
  return 0;
}
```

```bash
$ gcc test_code.c -o test_code
$ ./test_code
before swap: (5, 10)
after swap: (10, 5)
```

One common error with multi-line macros is to put a space after the \.  The compiler will give you an error for doing this.  Also, I put the \ so that they all line up vertically.  This just makes the code more readable - the compiler doesn't care.  A second error with multi-line macros is to put the \ after the last line.  The \ continues code to the next line.  It's an error to put the \ on the last line (which may or may not get reported by the compiler in a useful way).

Macros can be defined different ways depending upon another macro variable.

test_code.c
```c
#include <stdio.h>

#ifdef _DEBUG_
#define printx(x) printf( "DEBUG: %d\n", x)
#else
#define printx(x) printf( "NOT DEBUG: %d\n", x)
#endif

int main( int argc, char *argv[] ) {
  printx(5);
  return 0;
}
```

```bash
$ gcc test_code.c -o test_code
$ ./test_code
NOT DEBUG: 5
```

You can define compiler directives from the command line using the -D option in gcc.  Multiple directives can be defined by repeating the -D option.
```bash
$ gcc -D_DEBUG_ test_code.c -o test_code
$ ./test_code
DEBUG: 5
```

In creating the global allocator, maybe we only want line numbers and malloc to be passed in when the software is defined as being in debug mode.  For the rest of the project, we will consider code to be in debug mode if _STLA_DEBUG_ is defined.  To define stla_malloc, we might want to do the following.

```c
#ifdef _STLA_DEBUG_
#define stla_malloc(len) _stla_malloc_d(__FILE__, __FUNCTION__, __LINE__, len)
#else
#define stla_malloc(len) _stla_malloc(len)
#endif

void *_stla_malloc(size_t len);
void *_stla_malloc_d( const char *filename, const char *function, int line, size_t len );
```

When defining functions this way, it is good to come up with a convention.  My convention is to use an underscore before the function and to suffix the debug function with _d.  The interface will ultimately look like..

stla_global_allocator.h
```c
#ifndef _stla_global_allocator_H
#define _stla_global_allocator_H

/*
void *stla_malloc(size_t len);
void *stla_calloc(size_t len);
void *stla_realloc(void *p, size_t len);
char *stla_strdup(const char *p);
void stla_free(void *p);
*/

#include "impl/stla_global_allocator.h"

#endif
```

impl/stla_global_allocator.h
```c
#include <stdlib.h>

#ifdef _STLA_DEBUG_
#define stla_malloc(len) _stla_malloc_d(__FILE__, __FUNCTION__, __LINE__, len)
#define stla_calloc(len) _stla_calloc_d(__FILE__, __FUNCTION__, __LINE__, len)
#define stla_realloc(p, len) _stla_realloc_d(__FILE__, __FUNCTION__, __LINE__, p, len)
#define stla_strup(p) _stla_strdup_d(__FILE__, __FUNCTION__, __LINE__, p)
#define stla_free(p) _stla_free_d(__FILE__, __FUNCTION__, __LINE__, p)
#else
#define stla_malloc(len) _stla_malloc(len)
#define stla_calloc(len) _stla_calloc(len)
#define stla_realloc(p, len) _stla_realloc(p, len)
#define stla_strdup(p) _stla_strdup(p)
#define stla_free(p) _stla_free(p)
#endif

void *_stla_malloc(size_t len);
void *_stla_calloc(size_t len);
void *_stla_realloc(void *p, size_t len);
char *_stla_strdup(const char *p);
void _stla_free(void *p);

void *_stla_malloc_d(const char *filename, const char *function, int line, size_t len );
void *_stla_calloc_d(const char *filename, const char *function, int line, size_t len );
void *_stla_realloc_d(const char *filename, const char *function, int line, void *p, size_t len);
char *_stla_strdup_d(const char *filename, const char *function, int line, const char *p);
void _stla_free_d(const char *filename, const char *function, int line, void *p);
```

Ideally, when creating interfaces it is a good idea to hide implementation details.  Macros need to be defined in the header file in order for the code to work.  A common approach to hiding all of the macros is to define what the functions are in the main header file as a comment and then including another file which hides the details that are only needed to be known for advanced use cases.

# The Implementation

I typically will copy the header file to a c file and then begin the implementation.  In the case of non-debug versions of allocation, we could simply redefine our calls to call the system implementations.

impl/stla_global_allocator.h
```c
#include <stdlib.h>

#ifdef _STLA_DEBUG_
#define stla_malloc(len) _stla_malloc_d(__FILE__, __FUNCTION__, __LINE__, len)
#define stla_calloc(len) _stla_calloc_d(__FILE__, __FUNCTION__, __LINE__, len)
#define stla_realloc(p, len) _stla_realloc_d(__FILE__, __FUNCTION__, __LINE__, p, len)
#define stla_strup(p) _stla_strdup_d(__FILE__, __FUNCTION__, __LINE__, p)
#define stla_free(p) _stla_free_d(__FILE__, __FUNCTION__, __LINE__, p)
#else
#define stla_malloc(len) malloc(len)
#define stla_calloc(len) calloc(1, len)
#define stla_realloc(p, len) realloc(p, len)
#define stla_strdup(p) strdup(p)
#define stla_free(p) free(p)
#endif

void *_stla_malloc_d(const char *filename, const char *function, int line, size_t len );
void *_stla_calloc_d(const char *filename, const char *function, int line, size_t len );
void *_stla_realloc_d(const char *filename, const char *function, int line, void *p, size_t len);
char *_stla_strdup_d(const char *filename, const char *function, int line, const char *p);
void _stla_free_d(const char *filename, const char *function, int line, void *p);
```

We really only need to copy the functions which need implemented.
stla_global_allocator.c
```c
void *_stla_malloc_d(const char *filename, const char *function, int line, size_t len );
void *_stla_calloc_d(const char *filename, const char *function, int line, size_t len );
void *_stla_realloc_d(const char *filename, const char *function, int line, void *p, size_t len);
char *_stla_strdup_d(const char *filename, const char *function, int line, const char *p);
void _stla_free_d(const char *filename, const char *function, int line, void *p);
```

The first line should be to include the header file.
```c
#include "stla_global_allocator.h"

void *_stla_malloc_d(const char *filename, const char *function, int line, size_t len );
void *_stla_calloc_d(const char *filename, const char *function, int line, size_t len );
void *_stla_realloc_d(const char *filename, const char *function, int line, void *p, size_t len);
char *_stla_strdup_d(const char *filename, const char *function, int line, const char *p);
void _stla_free_d(const char *filename, const char *function, int line, void *p);
```




If you generally use objects like the one described above, it can significantly reduce errors and complexity with respect to allocation.  We can do more (a lot more).  What if our object could keep track of allocations and free memory for us?  This is generally the basis for languages which use garbage collection.  I'm going to illustrate how to build several objects which greatly reduce the number of calls to malloc and free.  

The first is an object which I've used for almost my whole career.  I've called it a pool (I'm not sure why :-)).  The basic idea with this object is that you can allocate and clear.  It has no free function.  When you clear, you clear all of the memory that has been previously allocated from the pool (internally, it is very efficient in that all it typically does is reset a counter to zero).  The base interface will look something like the following.

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

You may notice that I don't use two parameters for pool_calloc.  This is intentional as I don't see a benefit in changing the signature from malloc.  There is also no free method.  You can call the allocation methods as often as you like and they will remain valid until pool_clear or pool_destroy is called.  In the example above, we can modify it to use the pool for allocation.

# The Simplest Implementation

When approaching algorithms, it is often a good idea to think about the simplest implementation.  This implementation doesn't need to be efficient.  It can serve to prove that a more complicated version achieves the same result.

A very simple implementation of the pool interface described above is to have each allocation saved in a singly linked list on the pool_s structure.  The clear method could free all of the memory in the linked list and the destroy could free the pool_s object itself.  A singly linked list usually is linked to from a structure and then chained together using a member called next.  Each allocation would be placed in its own structure and linked to by the pool_s structure through the member head.  The two objects and implementation would probably look like the following.

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

We could use a trick like we did above and allocate the desired memory and the pool_node_t together.  If we did that we wouldn't need the void *m parameter either and the memory could just exist after the pool_node_t structure.

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

Using this simple structure, all of our allocations can be tracked and cleared by the pool object instead of requiring the application to remember where each allocation started from.  In our previous example, we can change the allocation to use the new pool object instead of malloc.

person.h
```c
#ifndef _person_H
#define _person_H

#include "pool.h"

typedef char person_t;

person_t *person_init(pool_t *pool, const char *first_name, const char *last_name);
void person_print(person_t *p);

// The destroy method is no longer needed!
// void person_destroy(person_t *p);

#endif
```

person.c
```c
#include "person.h"

person_t *person_init(pool_t *pool, const char *first_name, const char *last_name) {
  // allocate two strings plus the 2 zero terminators
  person_t *h = (person_t *)pool_malloc(pool, strlen(first_name) + strlen(last_name) + 2);
  char *p = (char *)(h);
  strcpy(p, first_name);
  p = p + strlen(p) + 1;
  strcpy(p, last_name);
  return h;
}

void person_print(person_t *h) {
  char *p = (char *)(h);
  printf( "%s %s\n", p, p+strlen(p)+1 );
}
```

test_person will also change to initialize a pool.

test_person.c
```c
#include "pool.h"
#include "person.h"

int main( int argc, char *argv[] ) {
  pool_t *pool = pool_init(256);
  for( int i=1; i<argc; i++ ) {
    pool_clear(pool);
    person_t *p = person_init(pool, argv[i-1], argv[i]);
    person_print(p);
  }
  pool_destroy(pool);
  return 0;
}
```

The above example modifies what the first and last name are to command line arguments.  Notice how the person_destroy is never needed.  The pool could be cleared at the bottom of the loop, but I've found that it is often easier to just clear it at the top of the loop.  In general, if you are going to write high quality code, you should emphasize repeated patterns over optimization if it really doesn't make a difference.  In the above case, calling clear at the end of the loop wouldn't change the number of times it is called.  What's cool is that even if you didn't clear the pool, the program would still work fine as long as the number of arguments wasn't so large that it exhausted memory.  The pool_destroy call would release all of the memory.  

# A better implementation of the pool

The pool is initialized with an initial_size of 256 in this example.  It likely could be much smaller.  In general, you want to initialize the pool with a size that captures the vast majority of cases.  It is okay if the pool requires extra memory.  We will build support for this when we build the pool.  This should become clearer as we move through the implementation.

Most of the time, when coding something I will start from an interface and then begin to fill in the blanks.  Sometimes, I will first explore challenging bits of code to understand them first.  I will often take the header file and save it as the implementation file and then modify.

pool.c
```c
#ifndef _pool_H
#define _pool_H

struct pool_s;
typedef struct pool_s pool_t;

pool_t *pool_init(size_t initial_size);
void *pool_malloc(pool_t *h, size_t len);
void *pool_calloc(pool_t *h, size_t len);
char *pool_strdup(pool_t *h, char *p);
void pool_clear(pool_t *h);
void pool_destroy(pool_t *h);

#endif
```

First, replace the #if.../#endif with an #include

pool.c
```c
#include "pool.h"

struct pool_s;
typedef struct pool_s pool_t;

pool_t *pool_init(size_t initial_size);
void *pool_malloc(pool_t *h, size_t len);
void *pool_calloc(pool_t *h, size_t len);
char *pool_strdup(pool_t *h, char *p);
void pool_clear(pool_t *h);
void pool_destroy(pool_t *h);
```

Next, begin to define your structure and/or your functions.  Perhaps code them in english first.

pool.c
```c
#include "pool.h"

struct pool_s {
  /*
    There needs to be memory reserved for all of the allocations and an offset into that memory.
    In addition to this, the allocations may overflow, so we may need extra blocks.  Those blocks
    might not be the same as the main block.  The order of the overflow blocks doesn't matter.  They
    can be allocated in reverse order, so we probably won't need to maintain a head and a tail.  The
    initial_size of the block should be the minimum growth size for additional nodes unless specified.  
    Perhaps, there could be a function to indicate growth size as different than the initial size.

    The main block probably will have buffer and length to maintain the initial block.
    char *buffer;
    size_t length;

    The offset will be an offset into the current node.  Perhaps it is more efficient if the initial
    block and length are put into an overflow?  To be explored...
    size_t offset;

    This could be used as an alternate size for new blocks beyond the initial block.
    size_t growth_size;

    What should current look like?  That'll be next.
    pool_node_t *current;
  */  
};

struct pool_node_s;
typedef struct pool_node_s pool_node_t;

struct pool_node_s {
  /* each node will have a buffer and length and a link to the previous block */
  char *buffer;
  size_t length;

  /* this will be NULL if it is the first block. */
  struct pool_node_s *prev;
};
...
```

In the above example, as I intentionally left extra fields and a somewhat contradictory set of statements in the pool_s structure.  I wanted to show how my thought process was developing.  One of the most important things I've learned about programming is that it is important to be able to think like the creator and not just consume the creation.  If you can get into the mind of the creator, you yourself can become a creator.  By the end, it became clear that I needed two structures to fully implement the pool and that buffer and length were not necessary in the main pool structure.  It became more obvious how I might clear the pool structure (free all memory except the first block).  I'll write it again a bit more concise.

pool.c
```c
#include "pool.h"

/*
  There needs to be memory reserved for all of the allocations and an offset into that memory.
  In addition to this, the allocations may overflow, so we may need extra blocks.  Those blocks
  might not be the same as the main block.  The order of the overflow blocks doesn't matter.  They
  can be allocated in reverse order, so we probably won't need to maintain a head and a tail.  The
  initial_size of the block should be the minimum growth size for additional nodes unless specified.  
  There will be a function to indicate growth size as different than the initial size.
*/
struct pool_node_s;
typedef struct pool_node_s pool_node_t;

struct pool_node_s {
  /* each node will have a buffer and length and a link to the previous block */
  char *buffer;
  size_t length;

  /* this will be NULL if it is the first block. */
  struct pool_node_s *prev;
};

struct pool_s {
  /* A pointer to the memory associated with the pool */
  pool_node_t *current;

  /* The offset will be an offset into the current node. */
  size_t offset;

  /* This could be used as an alternate size for new blocks beyond the initial block.  It will be
    initially set to the length of the first block and can later be modified. */
  size_t minimum_growth_size;
};
...
```

The implementation should be a bit easier now that the structure is pretty well defined.

```c
...

pool_t *pool_init(size_t initial_size) {
  if(initial_size == 0) abort(); /* this doesn't make any sense */
  /* Allocate the pool_t structure, the first node, and the memory in one call.  This
    keeps the memory in close proximity which is better for the CPU cache.  It also
    makes it so that when we destroy the handle, we only need to make one call to free
    for the handle, the first block, and the initial size. */  
  pool_t *h = (pool_t *)malloc(initial_size+sizeof(pool_t)+sizeof(pool_node_t));
  h->offset = 0;
  h->minimum_growth_size = initial_size;
  h->current = (pool_node_t *)(h+1);
  h->current->buffer = (char *)(h->current+1);
  h->current->length = initial_size;
  h->current->prev = NULL;
  return h;
}

/* Don't be afraid of long function names */
void pool_set_minimum_growth_size(pool_t *h, size_t growth_size) {
  if(growth_size == 0) abort();  /* this doesn't make sense */
  h->minimum_growth_size = growth_size;
}

void *pool_calloc(pool_t *h, size_t len) {
  /* calloc will simply call the pool_malloc function and then zero the memory. */
  void *dest = pool_malloc(h, len);
  memset(dest, 0, len);
  return dest;
}

char *pool_strdup(pool_t *h, char *p) {
  /* strdup will simply allocate enough bytes to hold the duplicated string,
    copy the string, and return the newly allocated string. */
  size_t len = strlen(p)+1;
  char *dest = (char *)pool_malloc(h, len);
  memcpy(dest, p, len);
  return dest;
}

void pool_clear(pool_t *h) {
  /* free all nodes which have a prev link.  If extra nodes haven't been used,
     this will break out of the loop immediately.  Once done, set the offset to
     zero to reuse the main block. */
  /* save a reference to the previous pointer on the current node */
  pool_node_t *prev = h->current->prev;
  /* the only way that prev would be NULL is if it is the main node */
  while(prev) {
    /* free the current block */
    free(h->current);
    /* set the current block to the previous block */
    h->current = prev;
    /* alter the reference to the previous block to the one on the new current block */
    prev = prev->prev;
  }
  /* set the offset to zero, so that pool_malloc will start over on the next call */
  h->offset = 0;
}

void pool_destroy(pool_t *h) {
  /* pool_clear frees all of the memory from all of the extra nodes and only leaves
    the main block and main node allocated */
  pool_clear(h);
  /* free the main block and the main node */
  free(h);
}
```

The allocation method (pool_malloc).  I left this out intentionally so that I could place more emphasis on it.  

```c
void *pool_malloc(pool_t *h, size_t len) {
  if(h->offset+len < h->current->length) {
    char *r = h->current->buffer + h->offset;
    h->offset += len;
    return r;
  }

  size_t block_size = len;
  if(block_size < h->minimum_growth_size)
    block_size = h->minimum_growth_size;
  pool_node_t *block = (pool_node_t *)malloc(sizeof(pool_node_t) + block_size);
  block->buffer = (char *)(block+1);
  block->length = block_size;
  block->prev = h->current;
  h->current = block;
  h->offset = len;
  return block->buffer;
}
```

The typical case is that the memory needed fits within the current block.
```c
if(h->offset+len < h->current->length) {
  char *r = h->current->buffer + h->offset;
  h->offset += len;
  return r;
}
```

If the memory doesn't fit within the current block, then a new block must be allocated.  The pseudocode would look like the following.

```
determine a block size based upon the maximum of the length requested and the
  minimum growth size.
allocate the pool_node_t structure and the block size together to avoid making
  two allocation calls (and subquently having to make multiple free calls).
set the buffer pointer to just after the pool_node_t structure.
set the length to the block size.
set the prev pointer to the current pointer on the pool handle.
set the current pointer on the pool handle to the new block.
set the offset to the length requested so that subsequent allocation calls will
 use memory after the given block.
the memory that you return will be the buffer since this is the first thing being
 allocated from the block.
 ```

The pool_node_s *prev forms a singly linked list in the above code.  The pool_node_t *current member in the main pool handle serves as the head of the linked list.  It is extremely common for me to use singly linked lists and code like the following to link items together putting the new element at the head of the list.

```c
block->prev = h->current;
h->current = block;
```

block->prev links to the h->current before h->current is set to the block.  This forms the singly linked list.  

# Memory Alignment

If you call pool_malloc(pool, 1) followed by pool_malloc(pool, 8), the offset will be 9 and the result of the second call will have unaligned memory.  This is fine if the memory doesn't need to be aligned.  Examples of memory that doesn't need aligned is pointers to strings (or sequences of a single byte).  In general, it is best to align memory to the OS bit size (the number of bits found in size_t).  The pool_malloc function should skip bytes to make its result aligned.  There are cases where allocating memory that is unaligned is better.  Imagine if you allocated the memory for every word in the english dictionary.  In this case, you would have many strings which are less than 8 bytes or less than 16 bytes.  There would be many holes.  If you use unaligned memory for this, you will save memory and since the strings by there nature are byte oriented, you won't suffer performance much if at all.  We could rename this function pool_umalloc and then make a new pool_malloc method that handles alignment.  The pool_strdup would likely want to call pool_umalloc.

In order to align the memory returned from pool_malloc, the offset should be moved to an aligned boundary before it is compared.  It is possible that the offset will extend beyond the end of the block from this call.  To avoid this from happening, you can make the initial_size round up to the nearest aligned number.  For example, if the pool was initialized with 14 bytes, you might change the initial_size to 16 to make it aligned.  This check would also exist for the minimum_growth_size when it is set.  The following code will align a number.

```c
size_t num = 10;
size_t aligned_num = num + ((sizeof(size_t)-(num & (sizeof(size_t)-1))) & (sizeof(size_t-1));
```

The new pool_malloc function.

```c
void *pool_malloc(pool_t *h, size_t len) {
  size_t o = h->offset + ((sizeof(size_t)-(h->offset & (sizeof(size_t)-1))) & (sizeof(size_t)-1));
  if(o+len < h->current->length) {
    char *r = h->current->buffer + o;
    h->offset = o+len;
    return r;
  }

  size_t block_size = len;
  if(block_size < h->minimum_growth_size)
    block_size = h->minimum_growth_size;
  pool_node_t *block = (pool_node_t *)malloc(sizeof(pool_node_t) + block_size);
  block->buffer = (char *)(block+1);
  block->length = block_size;
  block->prev = h->current;
  h->current = block;
  h->offset = len;
  return block->buffer;
}
```

Another useful alignment trick is to alter the pool size when the initial_size or minimum_growth_size is a multiple of 4096.  The idea is that if an even page size is requested, the user most likely would want the internal block to be part of the even page size.  The init function also needs to be aligned.

```c
pool_t *pool_init(size_t initial_size) {
  if(initial_size == 0) abort(); /* this doesn't make any sense */
  /* round initial_size up to be properly aligned */
  initial_size += ((sizeof(size_t)-(initial_size & (sizeof(size_t)-1))) & (sizeof(size_t)-1));

  /* Allocate the pool_t structure, the first node, and the memory in one call.  This
    keeps the memory in close proximity which is better for the CPU cache.  It also
    makes it so that when we destroy the handle, we only need to make one call to free
    for the handle, the first block, and the initial size. */  
  size_t block_size = initial_size;
  if((block_size & 4096) == 0)
    block_size -= (sizeof(pool_t)+sizeof(pool_node_t));

  pool_t *h = (pool_t *)malloc(block_size+sizeof(pool_t)+sizeof(pool_node_t));
  h->offset = 0;
  h->current = (pool_node_t *)(h+1);
  h->current->buffer = (char *)(h->current+1);
  h->current->length = block_size;
  h->current->prev = NULL;

  pool_set_minimum_growth_size(h, initial_size);
  return h;
}

/* Don't be afraid of long function names */
void pool_set_minimum_growth_size(pool_t *h, size_t growth_size) {
  if(growth_size == 0) abort();  /* this doesn't make sense */
  /* round growth_size up to be properly aligned */
  growth_size += ((sizeof(size_t)-(growth_size & (sizeof(size_t)-1))) & (sizeof(size_t)-1));

  /* if a multiple of 4096, then subtract the sizeof(pool_node_t) from it */
  if((growth_size & 4096) == 0)
    growth_size -= sizeof(pool_node_t);

  h->minimum_growth_size = growth_size;
}
```

# Removing buffer member from pool_node_s

One thing you may have noticed is that I free'd the blocks in pool_clear without worrying about the buffer inside of the block.  This is because I allocate the block and the buffer inside the block in a single call.  The memory for the buffer will be just after the pool_node_s structure.  If you consider this, then you might realize that there really isn't a need for the buffer pointer at all.  In addition to this, if you think about how pointers work, it may make more sense to convert the offset and the length of the buffers to pointers.  

Removing buffer involves removing it from the pool_node_s structure and the pool_init, pool_malloc, and pool_umalloc functions.  The pool_umalloc is the same as pool_malloc except that it doesn't do the alignment, so we will leave it out for now.

Notice that in pool_node_s and in pool_init, all you end up doing is eliminating the reference to buffer.  Sometimes, code can in some ways be simpler when it is more optimized.  I realize that the name of the memory isn't called buffer anymore, but it should be fairly easy to understand that it is the bytes after the pool_node_s structure.
```c
struct pool_node_s {
  /* each node will have a length and a link to the previous block */
  /* 1. remove the next line */
  // char *buffer;
  size_t length;

  /* this will be NULL if it is the first block. */
  struct pool_node_s *prev;
};

pool_t *pool_init(size_t initial_size) {
  ...
  pool_t *h = (pool_t *)malloc(block_size+sizeof(pool_t)+sizeof(pool_node_t));
  h->offset = 0;
  h->current = (pool_node_t *)(h+1);
  /* 2.  remove the next line */
  // h->current->buffer = (char *)(h->current+1);
  h->current->length = block_size;
  h->current->prev = NULL;

  pool_set_minimum_growth_size(h, initial_size);
  return h;
}
```

In pool_malloc, two of the changes (3 and 5) are to simple refer to the memory location by casting to the byte after the pool_node_s structure, and the other change (4) is to remove the reference.

```c
void *pool_malloc(pool_t *h, size_t len) {
  size_t o = h->offset + ((sizeof(size_t)-(h->offset & (sizeof(size_t)-1))) & (sizeof(size_t)-1));
  if(o+len < h->current->length) {
    /* 3.  The following line changes from */
    // char *r = h->current->buffer + o;
    /* To */
    char *r = (char *)(h->current+1) + o;
    h->offset = o+len;
    return r;
  }

  size_t block_size = len;
  if(block_size < h->minimum_growth_size)
    block_size = h->minimum_growth_size;
  pool_node_t *block = (pool_node_t *)malloc(sizeof(pool_node_t) + block_size);
  /* 4.  remove the next line */
  // block->buffer = (char *)(block+1);
  block->length = block_size;
  block->prev = h->current;
  h->current = block;
  h->offset = len;
  /* 5. The following line changes from */
  // return block->buffer;
  /* To */
  return (char *)(block+1);
}
```

This isn't a huge optimization, but it does save 8 bytes from every block and given that the pool object will likely be very important to your projects in the future, every little bit helps!  We might want to consider changing offset (will name it curp) and length (will name endp) to pointers.

The pool_node_s structure will change from
```c
struct pool_node_s {
  /* each node will have a length and a link to the previous block */
  size_t length;

  /* this will be NULL if it is the first block. */
  struct pool_node_s *prev;
};
```

To

```c
struct pool_node_s {
  /* each node will have a endp that refers to the end of the buffer and a link
    to the previous block */
  char *endp;

  /* this will be NULL if it is the first block. */
  struct pool_node_s *prev;
};
```

The pool_s structure will also change from

```c
struct pool_s {
  /* A pointer to the memory associated with the pool */
  pool_node_t *current;

  /* The offset will be an offset into the current node. */
  size_t offset;

  /* This could be used as an alternate size for new blocks beyond the initial block.  It will be
    initially set to the length of the first block and can later be modified. */
  size_t minimum_growth_size;
};
```

To

```c
struct pool_s {
  /* A pointer to the memory associated with the pool */
  pool_node_t *current;

  /* A pointer into the current node where memory is available. */
  char *curp;

  /* This could be used as an alternate size for new blocks beyond the initial block.  It will be
    initially set to the length of the first block and can later be modified. */
  size_t minimum_growth_size;
};
```

The init function will need to be modified.

```c
h->offset = 0;
```

gets changed to

```c
h->curp = (char *)(h->current+1);
```

```c
pool_t *pool_init(size_t initial_size) {
  ...
  pool_t *h = (pool_t *)malloc(block_size+sizeof(pool_t)+sizeof(pool_node_t));
  /* The following lines change from */
  // h->offset = 0;
  h->current = (pool_node_t *)(h+1);
  // h->current->length = block_size;
  /* To */
  h->current = (pool_node_t *)(h+1);
  h->curp = (char *)(h->current+1);
  h->current->endp = h->curp + block_size;
  h->current->prev = NULL;

  pool_set_minimum_growth_size(h, initial_size);
  return h;
}
```

The pool_init should look like the following...

```c
pool_t *pool_init(size_t initial_size) {
  if(initial_size == 0) abort(); /* this doesn't make any sense */
  /* round initial_size up to be properly aligned */
  initial_size += ((sizeof(size_t)-(initial_size & (sizeof(size_t)-1))) & (sizeof(size_t)-1));

  /* Allocate the pool_t structure, the first node, and the memory in one call.  This
    keeps the memory in close proximity which is better for the CPU cache.  It also
    makes it so that when we destroy the handle, we only need to make one call to free
    for the handle, the first block, and the initial size. */  
  size_t block_size = initial_size;
  if((block_size & 4096) == 0)
    block_size -= (sizeof(pool_t)+sizeof(pool_node_t));

  pool_t *h = (pool_t *)malloc(block_size+sizeof(pool_t)+sizeof(pool_node_t));
  h->current = (pool_node_t *)(h+1);
  h->curp = (char *)(h->current+1);
  h->current->endp = h->curp + block_size;
  h->current->prev = NULL;

  pool_set_minimum_growth_size(h, initial_size);
  return h;
}
```

The pool_clear method will change as well since it referenced offset

```c
void pool_clear(pool_t *h) {
  /* free all nodes which have a prev link.  If extra nodes haven't been used,
     this will break out of the loop immediately.  Once done, set the offset to
     zero to reuse the main block. */
  /* save a reference to the previous pointer on the current node */
  pool_node_t *prev = h->current->prev;
  /* the only way that prev would be NULL is if it is the main node */
  while(prev) {
    /* free the current block */
    free(h->current);
    /* set the current block to the previous block */
    h->current = prev;
    /* alter the reference to the previous block to the one on the new current block */
    prev = prev->prev;
  }
  /* The following line will change from */
  // h->offset = 0;
  /* To */
  h->curp = (char *)(h->current+1);
}
```

The pool_malloc (and pool_umalloc) will need to change.

Instead of tracking an offset, we will track a pointer.

```c
size_t o = h->offset + ((sizeof(size_t)-(h->offset & (sizeof(size_t)-1))) & (sizeof(size_t)-1));
```

changes to

```c
char *r = h->curp + ((sizeof(size_t)-((size_t)(h->curp) & (sizeof(size_t)-1))) & (sizeof(size_t)-1));
```

```c
if(o+len < h->current->length) {
```

changes to

```c
if(r+len < h->current->endp) {
```

Because we were doing pointer arithmetic, the if statement looks very similar.

Because r is already a pointer to the current place in RAM that we want to allocate, we don't need the following line.

```c
char *r = (char *)(h->current+1) + o;
```

We need to update curp much like we would offset.

```
h->offset = o+len;
```

changes to

```
h->curp = r+len;
```

The top part of pool_malloc looks like the following.

```c
void *pool_malloc(pool_t *h, size_t len) {
  char *r = h->curp + ((sizeof(size_t)-((size_t)(h->curp) & (sizeof(size_t)-1))) & (sizeof(size_t)-1));
  if(r+len < h->current->endp) {
    h->curp = r+len;
    return r;
  }

  size_t block_size = len;
  if(block_size < h->minimum_growth_size)
    block_size = h->minimum_growth_size;
  pool_node_t *block = (pool_node_t *)malloc(sizeof(pool_node_t) + block_size);
```

The rest of the change is very similar to what was done for the pool_init method.

```c
  block->length = block_size;
  block->prev = h->current;
  h->current = block;
  h->offset = len;
  return (char *)(block+1);
}
```

changes to

```c
  block->prev = h->current;
  h->current = block;
  r = (char *)(block+1);
  block->endp = r + block_size;
  h->curp = r + len;
  return r;
}
```

You should notice that as this code develops, the interface has changed very little.  Ideally, you create interfaces that allow your code to work better over time without other components which might use the library or object even noticing.  Hopefully, you can see how optimizing code for performance can sometimes be done without significantly sacrificing readability.  Pointer arithmetic is very efficient and if you use it right and follow consistent patterns, it can written in a bug free manner.

# Extending pool_strdup

If you consider the implementation of pool_strdup...

```c
char *pool_strdup(pool_t *h, char *p) {
  /* strdup will simply allocate enough bytes to hold the duplicated string,
    copy the string, and return the newly allocated string. */
  size_t len = strlen(p)+1;
  char *dest = (char *)pool_umalloc(h, len);
  memcpy(dest, p, len);
  return dest;
}
```

You might notice that we can create a use derivative of it that doesn't find the length of the string.  If the length was passed in as a parameter, then the length wouldn't have to be calculated and the data could be binary.  We will call this pool_dup...

```c
char *pool_dup(pool_t *h, char *data, size_t len) {
  /* dup will simply allocate enough bytes to hold the duplicated data,
    copy the data, and return the newly allocated memory which contains a copy of data.
    Because the data could need aligned, we will use pool_malloc instead of pool_umalloc */
  char *dest = (char *)pool_malloc(h, len);
  memcpy(dest, data, len);
  return dest;
}
```

The declaration would also be added to the pool.h file.

```c
char *pool_dup(pool_t *h, char *data, size_t len);
```

Many languages have the ability to format strings as part of the syntax.  For example, in JavaScript, you can do the following.

```javascript
var myname = "Andy Curtis";
var hello = "Hello " + myname;
var age = 40;
var age_string = "My age is " + age;
```

In the example above, the age_string allocates memory to hold the final string "My age is 40" and then copies that into the memory that was allocated.  C doesn't have the ability described above, but it does have a function which can print a formatted string either to the screen, a file, or to a block of memory.  This function is called printf, fprintf, or sprintf respectively.  These functions are built on the variable argument syntax that C supports natively through va_args.  

In C you might construct <b>age_string</b> above using the following code.

```c
int age = 40;
char *age_string = (char *)malloc(strlen("My age is ") + 10);
sprintf(age_string, "My age is %d", age);
// maybe print age_string to the screen using printf...
printf( "%s\n", age_string );
```

That's a lot of code.  What if you could create a function kind of like pool_strdup that allowed for formatting?  For example...

```c
char *age_string = pool_strdup_formatted(pool, "My age is %d", 40);
```

To be consistent with the other C function names, we could shorten pool_strdup_formatted to pool_strdupf.  Implementing a function which takes any parameters requires that you use variadic arguments.  I've included a couple of references to describe what variadic arguments are and how they work in C.

[vsprintf](https://www.tutorialspoint.com/c_standard_library/c_function_vsprintf)<br/>
[vsnprintf](https://c.happycodings.com/gnu-linux/code28.html)<br/>
[Variadic Macros](https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html)<br/>

Basically, in C, you can define a string (const char *) followed by a parameter named <b>...</b>.  The <b>...</b> must be the last parameter and must immediately follow the string.  The <b>...</b> represents that any parameter may be passed in.  The format string that precedes it, determines how the parameters are parsed.

The pool_strdupf function would have a function signature or declaration that looks like the following...

```c
char *pool_strdupf(pool_t *pool, const char *fmt, ...);
```

To access the variadic arguments, you need to use a few functions...

```c
va_list // this is the data type for all variable arguments.
va_start(va_list args, const char *fmt) // this takes in the va_list and fmt string
                                        // and finds the remaining parameters.
va_copy(va_list dest, va_list src) // copy argument lists
va_end(va_list args) // cleans up resources used by va_list
```

The pseudocode implementation
```
char *pool_strdupf(pool_t *pool, const char *fmt, ...) {
  associate a va_list type to the parameters after fmt.
  use whatever memory is leftover in the pool's current buffer for vsnprintf
    vsnprintf will return the size of the memory needed or -1 if it is unable to
    determine how much memory is needed.  Because of this, we may need to copy the
    associated list and use the copy.
  int n = vsnprintf( pool's leftover pointer, number of bytes leftover, fmt, args_copy );
  if n < 0, then abort
  destroy the copy of args
  if n < number of bytes leftover
    destroy args
    get a reference to the current offset in pool
    increment the current offset by n + 1 bytes (1 for the zero terminator)
    return the reference

  allocate n+1 bytes from pool (pool_umalloc) into return value
  int n2 = vsnprintf( return value, n+1, fmt, args )
  if n != n2, then abort
  destroy args

  return return value
}
```

In the above pseudocode, vsnprintf is called twice with args.  The advantage of vsnprintf is that other objects can create their own versions of sprintf as we are doing now.  We can expose our own version of vsnprintf to pool as pool_strdupvf and make pool_strdupf simpler.

The pseudocode implementation
```
char *pool_strdupvf(pool_t *pool, const char *fmt, va_list args) {
  associate a va_list type to the parameters after fmt to a variable called args.
  use whatever memory is leftover in the pool's current buffer for vsnprintf
    vsnprintf will return the size of the memory needed or -1 if it is unable to
    determine how much memory is needed.  Because of this, we may need to copy the
    associated list and use the copy.
  int n = vsnprintf( pool's leftover pointer, number of bytes leftover, fmt, args_copy );
  if n < 0, then abort
  destroy the copy of args
  if n < number of bytes leftover
    get a reference to the current offset in pool
    increment the current offset by n + 1 bytes (1 for the zero terminator)
    return the reference

  allocate n+1 bytes from pool (pool_umalloc) into return value
  copy the args again
  int n2 = vsnprintf( return value, n+1, fmt, args )
  if n != n2, then abort
  destroy the copy of args again

  return return value
}

char *pool_strdupf(pool_t *pool, const char *fmt, ...) {
  associate a va_list type to the parameters after fmt to a variable called args.
  char *res = pool_strdupvf( pool, fmt, args );
  destroy the args va_list
  return res
}
```

Now the C implementation...

In pool.c
```c
char *pool_strdupvf(pool_t *pool, const char *fmt, va_list args) {
  va_list args_copy;
  va_copy(args_copy, args);
  size_t leftover = pool->current->endp - pool->curp;
  char *r = pool->curp;
  int n=vsnprintf( r, leftover, fmt, args_copy );  
  if(n < 0) abort();
  va_end(args_copy);
  if(n < leftover) {
    pool->curp += leftover + 1;
    return r;
  }
  r = (char *)pool_umalloc(pool, n+1);
  va_copy(args_copy, args);
  int n2 = vsnprintf( r, n+1, fmt, args_copy );
  if((n+1) != n2) abort();  // should never happen!
  va_end(args_copy);
  return r;
}

char *pool_strdupf(pool_t *pool, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char *r = pool_strdupvf( pool, fmt, args );
  va_end(args);
  return r;
}
```

In pool.h
```c
#include <stdarg.h>

char *pool_strdupvf(pool_t *pool, const char *fmt, va_list args);
char *pool_strdupf(pool_t *pool, const char *fmt, ...);
```

# Keeping track of the pool's size

One of the advantages of using the pool object is that eliminates the need to track pointers and for the end user to call free on every object that is allocated.  Memory leaks can still occur if the user doesn't clear the pool as needed or if the pool isn't destroyed when it should be.  There are a couple of things that can be done to make it easier to find memory leaks.  One is to be able to know how much memory the pool has doled out and is taking up.  It's possible that the pool could have excessive memory which isn't getting used.  There are a few options for how to profile the pool, but a simple one is to simply track how many bytes have been allocated by the client and how many bytes have been allocated by the pool itself.  We will call the bytes allocated by the client <b>size</b> and the bytes allocated by the pool itself <b>used</b>.

The implementation of this is quite straight forward.

First add the two variables to the bottom of the pool_s structure.
```c
struct pool_s {
  ...
  size_t size;
  size_t used;
};
```

Set size and used to their initial values in pool_init.
```c
pool_t *pool_init(size_t initial_size) {
  ...
  pool_t *h = (pool_t *)malloc(initial_size+sizeof(pool_t)+sizeof(pool_node_t));
  h->used = initial_size+sizeof(pool_t)+sizeof(pool_node_t);
  h->size = 0;
  ...
}
```

Because size is a value that is meant for debugging and/or is informational, we will compute it and only update it's value when new blocks are added. pool_malloc and pool_umalloc will change to update the size and used if a new block is added.  Before adjusting h->current to point to the new block, subtract h->curp from the endp on h->current to get the number of bytes used in the last block.  

pool.c (pool_umalloc changes similarly)
```c
void *pool_malloc(pool_t *h, size_t len) {
  ...
  pool_node_t *block = (pool_node_t *)malloc(sizeof(pool_node_t) + block_size);
  h->used += sizeof(pool_node_t) + block_size;
  h->size += (h->current->endp - h->curp);
  ...
}
```

The pool_clear method will reset used and size.

```c
void pool_clear(pool_t *h) {
  pool_node_t *prev = h->current->prev;
  while(prev) {
    free(h->current);
    h->current = prev;
    prev = prev->prev;
  }
  h->curp = (char *)(h->current+1);
  h->size = 0;
  h->used = (h->current->endp-h->curp)+sizeof(pool_t)+sizeof(pool_node_t);
}
```

Finally, we can add functions to get the pool's size and used variables.

pool.c
```c
size_t pool_size(pool_t *h) { return h->size; }
size_t pool_used(pool_t *h) { return h->used; }
```

pool.h
```c
size_t pool_size(pool_t *h);
size_t pool_used(pool_t *h);
```

# Finishing the pool object

There are a few more things that should be done to make the pool object ready for prime time.  It is generally a good idea to prefix all of your functions with some package name to prevent them from conflicting with other packages.  Perhaps, we could prefix all of these functions with stla (standard template library alternative).

For example,
pool.h -> stla_pool.h
```c
#ifndef _stla_pool_H
#define _stla_pool_H

#include <stdarg.h>

struct stla_pool_s;
typedef struct stla_pool_s stla_pool_t;

stla_pool_t * stla_pool_init(size_t size);
...

#endif
```

Additionally, you may want to track which file, function, and lines calls to stla_pool_init, stla_pool_clear, and stla_pool_destroy come from.  This tracking might be something that is only enabled in when memory tracking is defined.  

For example,
stla_pool.h
```c
#ifdef MEMORY_TRACKING
#define stla_pool_init(size) _stla_pool_tracked_init(size, __FILE__, __FUNCTION__, __LINE__)
#define stla_pool_clear(h) _stla_pool_tracked_clear(h, __FILE__, __FUNCTION__, __LINE__)
#define stla_pool_destroy(h) _stla_pool_tracked_destroy(h, __FILE__, __FUNCTION__, __LINE__)
#else
#define stla_pool_init(size) _stla_pool_init(size)
#define stla_pool_clear(h) _stla_pool_clear(h)
#define stla_pool_destroy(h) _stla_pool_destroy(h)
#endif

struct stla_pool_s;
typedef struct stla_pool_s stla_pool_t;

stla_pool_t * _stla_pool_init(size_t size);
void _stla_pool_clear(stla_pool_t *h);
void _stla_pool_destroy(stla_pool_t *h);

/* tracked versions */
stla_pool_t * _stla_pool_tracked_init(size_t size, const char *filename, const char *function, int line);
void _stla_pool_tracked_clear(stla_pool_t *h, const char *filename, const char *function, int line);
void _stla_pool_tracked_destroy(stla_pool_t *h, const char *filename, const char *function, int line);

...

#endif
```

stla_pool.c
```c
stla_pool_t * _stla_pool_tracked_init(size_t size, const char *filename, const char *function, int line) {
  /* we will implement tracking later */
  return _stla_pool_init(size);
}
void _stla_pool_tracked_clear(stla_pool_t *h, const char *filename, const char *function, int line) {
  /* we will implement tracking later */
  _stla_pooL_clear(h);  
}
void _stla_pool_tracked_destroy(stla_pool_t *h, const char *filename, const char *function, int line) {
  /* we will implement tracking later */
  _stla_pool_destroy(h);
}
```

For the functions we wish to redefine, we can place an underscore before them and then let the macro define which function is actually called.  We haven't implemented the _stla_pool_tracked_init function fully yet.  I will show how it is implemented after the stla_map object is described.  For now, they are stubbed out to make them functional.

The full version of stla_pool.h and stla_pool.c are found https://github.com/contactandyc/realtime/library/
