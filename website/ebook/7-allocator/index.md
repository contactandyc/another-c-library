---
path: "/7-allocator"
posttype: "tutorial"
title: "7. The Global Allocator Object"
---

A quick note about C allocation methods:

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

It's important to realize that malloc (memory alloc) is the core behind all of these functions or at a minimum, they are derivatives of malloc.

One of the reasons that people tend to steer clear of C is because you must maintain pointers to memory that you allocate so that it can later be freed.  In the last chapter, I introduced doubly-linked lists.  We can implement a way of tracking allocations using a doubly-linked list.  The functions that we will implement are the ones defined above - so the interface will be pretty straight forward.  To prevent naming conflicts, we will use the ac prefix.

```c
#ifndef _ac_allocator_H
#define _ac_allocator_H

#include "ac_common.h"

void *ac_malloc(size_t len);
void *ac_calloc(size_t len);
void *ac_realloc(void *p, size_t len);
char *ac_strdup(char *p);
void ac_free(void *p);

#endif
```

You may notice that I don't use two parameters for ac\_calloc.  This is intentional as I don't see a benefit in changing the signature from malloc.  

We could redefine our interface as

```c
#ifndef _ac_allocator_H
#define _ac_allocator_H

#include <stdlib.h> /* for size_t */

void *ac_malloc(const char *filename, int line, size_t len);
void *ac_calloc(const char *filename, int line, size_t len);
void *ac_realloc(const char *filename, int line, void *p, size_t len);
char *ac_strdup(const char *filename, int line, char *p);
void ac_free(const char *filename, void *p);

#endif
```

and then call our functions like this.

sample.c (this won't work - because I'm not building the object this way in the end)
```c
#include "ac_allocator.h"

int main( int argc, char *argv[] ) {
  char *s = (char *)ac_malloc(__FILE__, __LINE__, 100);
  ac_free(__FILE__, __LINE__, s);
  return 0;
}
```

There is a pattern above in that every time we call ac\_malloc (and others), we would call it with \_\_FILE\_\_, \_\_LINE\_\_, as the first two parameters.  

In creating the global allocator, maybe we only want line numbers and malloc to be passed in when the software is defined as being in debug mode.  To define ac\_malloc, we might want to do the following.  Considering that we have just identified how to merge the \_\_FILE\_\_ and \_\_LINE\_\_, we will use that.

```c
#ifdef _AC_DEBUG_MEMORY_
#define ac_malloc(len) _ac_malloc_d(__AC_FILE_LINE__, len)
#else
#define ac_malloc(len) malloc(len)
#endif

void *_ac_malloc_d( const char *caller, size_t len );
```

When defining functions this way, it is good to come up with a convention.  My convention is to use an underscore before the function and to suffix the debug function with \_d.  In defining the final interface, I came up with a few additional features.  The first is based upon the idea that the allocator will use an internal object to represent its structure.  That object will be global, and all of the allocation functions will have to pass a pointer to the object.

```c
void *_ac_malloc_d( const char *caller, size_t len );
```

becomes
```c
void *_ac_malloc_d( ac_allocator_t *a, const char *caller, size_t len );
```

The macro changes to.
```c
#define ac_malloc(len) _ac_malloc_d(NULL, __AC_FILE_LINE__, len)
```

A NULL allocator simply means that the global allocator will be used.

The second feature is to allow for a mechanism to allow for custom content in place of the caller.  For example, if we allocate a buffer and keep changing its size, we might want to know the following: where the buffer was initialized, the maximum size of the buffer, its initial size, etc. Passing a boolean at the end denotes this custom feature, which defaults to false.

```c
void *_ac_malloc_d( ac_allocator_t *a, const char *caller, size_t len );
```

becomes
```c
void *_ac_malloc_d( ac_allocator_t *a, const char *caller, size_t len, bool custom );
```

The macro changes to.
```c
#define ac_malloc(len) _ac_malloc_d(NULL, __AC_FILE_LINE__, len, false)
```

If the custom feature is enabled, the objects need a way to dump their state to a file (or to the terminal).  In C, you can define a function pointer and then associate the pointer to functions programmatically.  An example is below.  The gist of it is that you specify the new function pointer type by surrounding the name in parenthesis and an extra asterisk.

For example,
```c
typedef void (*my_function)();
```

would declare a function pointer type named my\_function, which takes no arguments and does not return anything (it has a void return type).  I recommend using a suffix for function pointers (I'm going to use \_f).  The allocator needs to define a function pointer to allow other objects to dump their details.

```c
typedef void (*ac_dump_details_f)(FILE *out, void *p, size_t length);
```

In C, the order of the members of a structure is also the byte order of the structure.  This is often exploited to allow a structure to be able to be cast as a structure of a different type.

```c
typedef struct {
  ac_dump_details_f dump;
  const char *caller;
} ac_allocator_dump_t;
```

Another useful feature is to have the state of the memory usage dumped every so often.  All this means is that if \_AC\_DEBUG\_MEMORY\_ is defined as a filename, any program which uses the allocator will record all of the memory allocations every N seconds and rotate the previous snapshot.  To maintain a smaller number of output files, the rotations will rotate files with an ever-expanding gap between them.

The full allocator interface is below.

$ac/src/ac\_allocator.h
```c
#ifndef _ac_allocator_H
#define _ac_allocator_H

#include "ac_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _AC_DEBUG_MEMORY_
#define ac_malloc(len) _ac_malloc_d(NULL, __AC_FILE_LINE__, len, false)
#define ac_calloc(len) _ac_calloc_d(NULL, __AC_FILE_LINE__, len, false)
#define ac_realloc(p, len) _ac_realloc_d(NULL, __AC_FILE_LINE__, p, len, false)
#define ac_strdup(p) _ac_strdup_d(NULL, __AC_FILE_LINE__, p)
#define ac_free(p) _ac_free_d(NULL, __AC_FILE_LINE__, p)
#else
#define ac_malloc(len) malloc(len)
#define ac_calloc(len) calloc(1, len)
#define ac_realloc(p, len) realloc(p, len)
#define ac_strdup(p) strdup(p)
#define ac_free(p) free(p)
#endif

typedef void (*ac_dump_details_f)(FILE *out, void *p, size_t length);

typedef struct {
  ac_dump_details_f dump;
  const char *caller;
} ac_allocator_dump_t;

struct ac_allocator_s;
typedef struct ac_allocator_s ac_allocator_t;

ac_allocator_t *ac_allocator_init();
void ac_allocator_destroy(ac_allocator_t *a);

void ac_dump_global_allocations(ac_allocator_t *a, FILE *out);

void *_ac_malloc_d(ac_allocator_t *a, const char *caller, size_t len, bool custom );

void *_ac_calloc_d(ac_allocator_t *a, const char *caller, size_t len, bool custom );

void *_ac_realloc_d(ac_allocator_t *a, const char *caller, void *p, size_t len, bool custom);

char *_ac_strdup_d(ac_allocator_t *a, const char *caller, const char *p );

void _ac_free_d(ac_allocator_t *a, const char *caller, void *p);

#endif
```

Before understanding the implementation, let's see how other objects and code use this.  The ac\_timer.h/c will change in the following way:

Include ac\_common.h

ac\_timer.h
```c
#include "ac_common.h"
```

Replace
```c
ac_timer_t *ac_timer_init(int repeat);
```

with
```c
#ifdef _AC_DEBUG_MEMORY_
#define ac_timer_init(repeat) _ac_timer_init(repeat, AC_FILE_LINE_MACRO("ac_timer"))
ac_timer_t *_ac_timer_init(int repeat, const char *caller);
#else
#define ac_timer_init(repeat) _ac_timer_init(repeat)
ac_timer_t *_ac_timer_init(int repeat);
#endif
```

The above code has two basic cases: one where \_AC\_DEBUG\_MEMORY\_ is defined, and one where it is not (#else).  It may be easier to break this into a couple of steps.

1.  Convert the init function to be prefixed with an underscore
```c
ac_timer_t *_ac_timer_init(int repeat);
```

2.  Create a macro which defines ac\_timer\_init as \_ac\_timer\_init
```c
#define ac_timer_init(repeat) _ac_timer_init(repeat)
ac_timer_t *_ac_timer_init(int repeat);
```

3.  Define the macro if logic with the else part filled in.
```c
#ifdef _AC_DEBUG_MEMORY_
#else
#define ac_timer_init(repeat) _ac_timer_init(repeat)
ac_timer_t *_ac_timer_init(int repeat);
#endif
```

4.  Add `const char *caller` to the debug version of \_ac\_timer\_init
```c
ac_timer_t *_ac_timer_init(int repeat, const char *caller);
```

5.  Define the macro to call the init function.
```c
#define ac_timer_init(repeat) _ac_timer_init(repeat, AC_FILE_LINE_MACRO("ac_timer"))
```

6.  Put the two calls in the #ifdef \_AC\_DEBUG\_MEMORY\_ section.
```c
#ifdef _AC_DEBUG_MEMORY_
#define ac_timer_init(repeat) _ac_timer_init(repeat, AC_FILE_LINE_MACRO("ac_timer"))
ac_timer_t *_ac_timer_init(int repeat, const char *caller);
#else
#define ac_timer_init(repeat) _ac_timer_init(repeat)
ac_timer_t *_ac_timer_init(int repeat);
#endif
```

Objects will typically use the AC\_FILE\_LINE\_MACRO("object_name") when defining the init call, as in step 5 above.

Change ac\_timer.c from
```c
ac_timer_t *ac_timer_init(int repeat) {
  ac_timer_t *t = (ac_timer_t *)malloc(sizeof(ac_timer_t));
```

to
```c
#ifdef _AC_DEBUG_MEMORY_
ac_timer_t *_ac_timer_init(int repeat, const char *caller) {
  ac_timer_t *t =
    (ac_timer_t *)_ac_malloc_d(NULL, caller,
                                   sizeof(ac_timer_t), false);
#else
ac_timer_t *_ac_timer_init(int repeat) {
  ac_timer_t *t = (ac_timer_t *)ac_malloc(sizeof(ac_timer_t));
#endif
```

1.  Change all malloc, calloc, strdup, realloc, and free calls to have ac\_ prefix.
2.  Change the function name from ac\_timer\_init to \_ac\_timer\_init.
3.  Wrap the block in a #ifdef \_AC\_DEBUG\_MEMORY\_/#else/#endif block.
4.  Define the \_AC\_DEBUG\_MEMORY\_ portion.  The \_ac\_timer\_init function has the extra const char *caller parameter.  The allocation uses \_ac\_malloc\_d directly, as shown above.

To test these changes out, I've modified code from $ac/illustrations/2\_timing/13\_timer

The following code is found in <i>illustrations/6\_allocator/1\_timing</i>
```
cd $ac/illustrations/6_allocator/1_timing
```

```
$ make
gcc -O3 -I../../../src -D_AC_DEBUG_MEMORY_=NULL ../../../src/ac_timer.c ../../../src/ac_allocator.c ../../../src/ac_buffer.c ../../../src/ac_pool.c test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 7.7260ns
Reverse => esreveR
time_spent: 1.8180ns
overall time_spent: 9.5440ns
----Date/Time----
99 byte(s) allocated in 4 allocations (160 byte(s) overhead)
test_timer.c:24: 27
test_timer.c:26 [ac_timer]: 32
test_timer.c:24: 8
test_timer.c:26 [ac_timer]: 32
```

Include ac\_allocator.h in test\_timer.c
```c
#include "ac_allocator.h"
```

and alter the malloc and free calls to ac\_malloc and ac\_free.  I've intentionally commented out several destroy calls and the ac\_free call.

When `make` was run above, the following extra lines were output
```
----Date/Time----
99 byte(s) allocated in 4 allocations (160 byte(s) overhead)
test_timer.c:24: 27
test_timer.c:26 [ac_timer]: 32
test_timer.c:24: 8
test_timer.c:26 [ac_timer]: 32
```

This output was generated with the `cat memory.log` command in the Makefile and indicates that 4 allocations were not properly freed.

line 24
```c
char *s = (char *)ac_malloc(len+1);
```

line 26
```c
ac_timer_t *copy_timer = ac_timer_init(ac_timer_get_repeat(overall_timer));
```

To fix the code, we need to make sure that copy\_timer is destroyed and s is freed.  Lines 47-48 are what need uncommented to make this work.
```c
// ac_timer_destroy(copy_timer);
// ac_free(s);
```

The error logged the ac\_timer\_init line as opposed to the ac\_malloc inside of ac\_timer_init.  This is likely more useful unless you are testing the individual object.

Go ahead and uncomment those lines and run `make` again
```
$ make
gcc -O3 -I../../../src -D_AC_DEBUG_MEMORY_=NULL ../../../src/ac_timer.c ../../../src/ac_allocator.c ../../../src/ac_buffer.c ../../../src/ac_pool.c test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 7.7260ns
Reverse => esreveR
time_spent: 1.8180ns
overall time_spent: 9.5440ns
----Date/Time----
```

Now the memory issues are fixed and only the date/time are printed from the memory.log file

The memory debug flag may be toggled in the Makefile which will determine whether a memory.log file is generated or not.

Memory-debug mode
```Makefile
FLAGS += -D_AC_DEBUG_MEMORY_=\"memory.log\"
```

Non-memory-debug mode
```Makefile
FLAGS += -D_AC_DEBUG_MEMORY_=NULL
```

If you view the test\_timer.c code, you will notice that a significant portion of the main function now has the ac\_ prefix.

test\_timer.c (main function)
```c
int main( int argc, char *argv[]) {
  int repeat_test = 1000000;
  ac_timer_t *overall_timer = ac_timer_init(repeat_test);
  for( int i=1; i<argc; i++ ) {
    size_t len = strlen(argv[i]);
    char *s = (char *)ac_malloc(len+1);

    ac_timer_t *copy_timer = ac_timer_init(ac_timer_get_repeat(overall_timer));
    ac_timer_start(copy_timer);
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
    }
    ac_timer_stop(copy_timer);

    ac_timer_t *test_timer = ac_timer_init(ac_timer_get_repeat(overall_timer));
    ac_timer_start(test_timer);
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
      reverse_string(s, len);
    }
    ac_timer_stop(test_timer);
    ac_timer_subtract(test_timer, copy_timer);
    ac_timer_add(overall_timer, test_timer);

    printf("%s => %s\n", argv[i], s);
    printf( "time_spent: %0.4fns\n", ac_timer_ns(test_timer) );

    ac_timer_destroy(test_timer);
    // ac_timer_destroy(copy_timer);
    // ac_free(s);
  }
  printf( "overall time_spent: %0.4fns\n", ac_timer_ns(overall_timer) );
  ac_timer_destroy(overall_timer);
  return 0;
}
```

Breaking out all of the ac\_ statements:
```c
  ac_timer_t *overall_timer = ac_timer_init(repeat_test);
    char *s = (char *)ac_malloc(len+1);

    ac_timer_t *copy_timer = ac_timer_init(ac_timer_get_repeat(overall_timer));
    ac_timer_start(copy_timer);

    ac_timer_stop(copy_timer);

    ac_timer_t *test_timer = ac_timer_init(ac_timer_get_repeat(overall_timer));
    ac_timer_start(test_timer);

    ac_timer_stop(test_timer);
    ac_timer_subtract(test_timer, copy_timer);
    ac_timer_add(overall_timer, test_timer);

    ac_timer_destroy(test_timer);
    // ac_timer_destroy(copy_timer);
    // ac_free(s);
  ac_timer_destroy(overall_timer);
```

If you mentally note that ac is just a prefix and view the code as
```c
timer_t *overall_timer = timer_init(repeat_test);
  char *s = (char *)malloc(len+1);

  timer_t *copy_timer = timer_init(timer_get_repeat(overall_timer));
  timer_start(copy_timer);

  timer_stop(copy_timer);

  timer_t *test_timer = timer_init(timer_get_repeat(overall_timer));
  timer_start(test_timer);

  timer_stop(test_timer);
  timer_subtract(test_timer, copy_timer);
  timer_add(overall_timer, test_timer);

  timer_destroy(test_timer);
  // timer_destroy(copy_timer);
  // free(s);
timer_destroy(overall_timer);
```

You can see that there are timer objects, malloc, and free.  Given that malloc, calloc, realloc, strdup, and free are so common in code, I opted not to provide any extra qualifiers other than ac.  I aim to make code highly optimized and very readable.  Another important feature of qualified naming is that it makes it possible to search for all places something exists.  For example, to find all cases where ac\_timer are used, you can run..

```
cd $ac/illustrations
grep -rn ac_timer .
```  

and your output might look like...
```
./2_timing/12_timer/test_timer.c:1:#include "ac_timer.h"
./2_timing/12_timer/test_timer.c:20:  ac_timer_t *overall_timer = ac_timer_init(repeat_test);
./2_timing/12_timer/test_timer.c:25:    ac_timer_t *copy_timer = ac_timer_init(ac_timer_get_repeat(overall_timer));
./2_timing/12_timer/test_timer.c:26:    ac_timer_start(copy_timer);
./2_timing/12_timer/test_timer.c:30:    ac_timer_stop(copy_timer);
./2_timing/12_timer/test_timer.c:32:    ac_timer_t *test_timer = ac_timer_init(ac_timer_get_repeat(overall_timer));
./2_timing/12_timer/test_timer.c:33:    ac_timer_start(test_timer);
./2_timing/12_timer/test_timer.c:38:    ac_timer_stop(test_timer);
./2_timing/12_timer/test_timer.c:39:    ac_timer_subtract(test_timer, copy_timer);
./2_timing/12_timer/test_timer.c:40:    ac_timer_add(overall_timer, test_timer);
./2_timing/12_timer/test_timer.c:43:    printf( "time_spent: %0.4fns\n", ac_timer_ns(test_timer) );
./2_timing/12_timer/test_timer.c:45:    ac_timer_destroy(test_timer);
./2_timing/12_timer/test_timer.c:46:    ac_timer_destroy(copy_timer);
./2_timing/12_timer/test_timer.c:49:  printf( "overall time_spent: %0.4fns\n", ac_timer_ns(overall_timer) );
./2_timing/12_timer/test_timer.c:50:  ac_timer_destroy(overall_timer);
./2_timing/12_timer/Makefile:2:OBJECTS=$(ROOT)/src/ac_timer.c
./2_timing/12_timer/Makefile:3:HEADER_FILES=$(ROOT)/src/ac_timer.h
Binary file ./2_timing/12_timer/test_timer matches
./2_timing/13_timer/test_timer.c:1:#include "ac_timer.h"
./2_timing/13_timer/test_timer.c:20:  ac_timer_t *overall_timer = ac_timer_init(repeat_test);
./2_timing/13_timer/test_timer.c:25:    ac_timer_t *copy_timer = ac_timer_init(ac_timer_get_repeat(overall_timer));
./2_timing/13_timer/test_timer.c:26:    ac_timer_start(copy_timer);
./2_timing/13_timer/test_timer.c:30:    ac_timer_stop(copy_timer);
...
```

This feature alone is extremely valuable when working with a large codebase.  It takes longer to write each line of code, but the reader can easily find every line of code where the object is used.  This makes it easier to find example code and to build upon the work of others.

## A Quick Recap

1.  The 5 basic allocation methods in C are...
```c
void *malloc(size_t len);
void *calloc(size_t num, size_t len);
void *realloc(void *p, size_t len);
char *strdup(char *p);
void free(void *p);
```

2.  We define those functions with the ac\_ prefix such that if we are debugging memory, we can track where the allocations are made.  This allows us to recognize memory leaks and potentially a couple of other common errors.

3.  Callback functions should have a suffix of \_f

4.  We defined an approach to find when objects are created (and not destroyed).  The basic changes to ac\_timer were outlined to make it support using the ac\_allocator.

5.  When we are using the ac\_allocator object outside of objects, the only change to the code is to replace the 5 basic allocation methods with `ac_...` If, for some reason, you do not wish to allocate memory using the ac method, then make sure that you don't free it with the ac method.

6.  We can define \_AC\_DEBUG\_MEMORY\_ as NULL and have memory leaks reported to the terminal when the program exits.

```Makefile
FLAGS += -D_AC_DEBUG_MEMORY_=\"memory.log\"
```

7.  We can define \_AC\_DEBUG\_MEMORY\_ as a string and have memory leaks reported to a file periodically.  The period is defined in seconds as \_AC\_DEBUG\_MEMORY\_SPEED\_ and defaults to 60 in ac\_common.h

8.  You can `grep` for any line of code which contains an object using the following approach.  The following `grep` line will search all subdirectories for the string ac\_timer and report the filename and line number where the text is found.

```
grep -rn ac_timer .
```

ac\_timer can be replaced with a function name or another object name (or whatever you want to find).

[Table of Contents (only if viewing on Github)](../../../README.md)
