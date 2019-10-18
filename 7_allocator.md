[Table of Contents](README.md)  - Copyright 2019 Andy Curtis

# The Global Allocator Object

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

It's important to realize that malloc (memory alloc) is the core behind all of these functions or at a minimum, they are derivates of malloc.

One of the reasons that people tend to steer clear of C is because you must maintain pointers to memory that you allocate so that it can later be freed.  In the last chapter, I introduced doubly-linked lists.  We can implement a way of tracking allocations using a doubly-linked list.  The functions that we will implement are the ones defined above - so the interface will be pretty straight forward.  To prevent naming conflicts, we will use the ac prefix.

```c
#ifndef _acallocator_H
#define _acallocator_H

#include "accommon.h"

void *acmalloc(size_t len);
void *accalloc(size_t len);
void *acrealloc(void *p, size_t len);
char *acstrdup(char *p);
void acfree(void *p);

#endif
```

You may notice that I don't use two parameters for accalloc.  This is intentional as I don't see a benefit in changing the signature from malloc.  

We could redefine our interface as

```c
#ifndef _acallocator_H
#define _acallocator_H

#include <stdlib.h> /* for size_t */

void *acmalloc(const char *filename, int line, size_t len);
void *accalloc(const char *filename, int line, size_t len);
void *acrealloc(const char *filename, int line, void *p, size_t len);
char *acstrdup(const char *filename, int line, char *p);
void acfree(const char *filename, void *p);

#endif
```

and then call our functions like this.

sample.c (this won't work - because I'm not building the object this way in the end)
```c
#include "acallocator.h"

int main( int argc, char *argv[] ) {
  char *s = (char *)acmalloc(__FILE__, __LINE__, 100);
  acfree(__FILE__, __LINE__, s);
  return 0;
}
```

There is a pattern above in that every time we call acmalloc (and others), we would call it with __FILE__, __LINE__, as the first two parameters.  

In creating the global allocator, maybe we only want line numbers and malloc to be passed in when the software is defined as being in debug mode.  To define acmalloc, we might want to do the following.  Considering that we have just identified how to merge the __FILE__ and __LINE__, we will use that.

```c
#ifdef _ACDEBUG_MEMORY_
#define acmalloc(len) _acmalloc_d(__ACFILE_LINE__, len)
#else
#define acmalloc(len) malloc(len)
#endif

void *_acmalloc_d( const char *caller, size_t len );
```

When defining functions this way, it is good to come up with a convention.  My convention is to use an underscore before the function and to suffix the debug function with _d.  In defining the final interface, I came up with a few additional features.  The first is based upon the idea that the allocator will use an internal object to represent its structure.  That object will be global, and all of the allocation functions will have to pass a pointer to the object.

```c
void *_acmalloc_d( const char *caller, size_t len );
```

becomes
```c
void *_acmalloc_d( acallocator_t *a, const char *caller, size_t len );
```

The macro changes to.
```c
#define acmalloc(len) _acmalloc_d(NULL, __ACFILE_LINE__, len)
```

A NULL allocator simply means to use the global allocator.

The second feature is to allow for a mechanism to allow for custom content in place of the caller.  For example, if we allocate a buffer and keep changing its size, we might want to know the following: where the buffer was initialized, the maximum size of the buffer, its initial size, etc. Passing a boolean at the end denotes this custom feature, which defaults to false.

```c
void *_acmalloc_d( acallocator_t *a, const char *caller, size_t len );
```

becomes
```c
void *_acmalloc_d( acallocator_t *a, const char *caller, size_t len, bool custom );
```

The macro changes to.
```c
#define acmalloc(len) _acmalloc_d(NULL, __ACFILE_LINE__, len, false)
```

If the custom feature is enabled, the objects need a way to dump their state to a file (or to the terminal).  In C, you can define a function pointer and then associate the pointer to functions programmatically.  An example is below.  The gist of it is that you specify the new function pointer type by surrounding the name in parenthesis and an extra asterisk.

For example,
```c
typedef void (*my_function)();
```

would declare a function pointer type named my_function, which took no arguments and didn't return anything (it has a void return type).  I recommend using a suffix for function pointers (I'm going to use _f).  The allocator needs to define a function pointer to allow other objects to dump their details.

```c
typedef void (*acdump_details_f)(FILE *out, void *p, size_t length);
```

In C, the order of the members of a structure is also the byte order of the structure.  This is often exploited to allow a structure to be able to be cast as a structure of a different type.

```c
typedef struct {
  acdump_details_f dump;
  const char *caller;
} acallocator_dump_t;
```

Another useful feature is to have the state of the memory usage dumped every so often.  All this means is that if _ACDEBUG_MEMORY_ is defined as a filename, any program which uses the allocator will record all of the memory allocations every N seconds and rotate the previous snapshot.  To maintain a smaller number of output files, the rotations will rotate files with an ever-expanding gap between them.

The full allocator interface is below.

$ac/src/acallocator.h
```c
#ifndef _acallocator_H
#define _acallocator_H

#include "accommon.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _ACDEBUG_MEMORY_
#define acmalloc(len) _acmalloc_d(NULL, __ACFILE_LINE__, len, false)
#define accalloc(len) _accalloc_d(NULL, __ACFILE_LINE__, len, false)
#define acrealloc(p, len) _acrealloc_d(NULL, __ACFILE_LINE__, p, len, false)
#define acstrdup(p) _acstrdup_d(NULL, __ACFILE_LINE__, p)
#define acfree(p) _acfree_d(NULL, __ACFILE_LINE__, p)
#else
#define acmalloc(len) malloc(len)
#define accalloc(len) calloc(1, len)
#define acrealloc(p, len) realloc(p, len)
#define acstrdup(p) strdup(p)
#define acfree(p) free(p)
#endif

typedef void (*acdump_details_f)(FILE *out, void *p, size_t length);

typedef struct {
  acdump_details_f dump;
  const char *caller;
} acallocator_dump_t;

struct acallocator_s;
typedef struct acallocator_s acallocator_t;

acallocator_t *acallocator_init();
void acallocator_destroy(acallocator_t *a);

void acdump_global_allocations(acallocator_t *a, FILE *out);

void *_acmalloc_d(acallocator_t *a, const char *caller, size_t len, bool custom );

void *_accalloc_d(acallocator_t *a, const char *caller, size_t len, bool custom );

void *_acrealloc_d(acallocator_t *a, const char *caller, void *p, size_t len, bool custom);

char *_acstrdup_d(acallocator_t *a, const char *caller, const char *p );

void _acfree_d(acallocator_t *a, const char *caller, void *p);

#endif
```

Before understanding the implementation, let's see how other objects and code use this.  The actimer.h/c will change in the following way:

Include accommon.h

actimer.h
```c
#include "accommon.h"
```

Replace
```c
actimer_t *actimer_init(int repeat);
```

with
```c
#ifdef _ACDEBUG_MEMORY_
#define actimer_init(repeat) _actimer_init(repeat, ACFILE_LINE_MACRO("actimer"))
actimer_t *_actimer_init(int repeat, const char *caller);
#else
#define actimer_init(repeat) _actimer_init(repeat)
actimer_t *_actimer_init(int repeat);
#endif
```

The above code has two basic cases: one where _ACDEBUG_MEMORY_ is defined, and one where it is not (#else).  It may be easier to break this into a couple of steps.

1.  convert the init function to be prefixed with an underscore

```c
actimer_t *actimer_init(int repeat);
```

becomes
```c
actimer_t *_actimer_init(int repeat);
```

2.  create a macro which defines actimer_init as _actimer_init
```c
#define actimer_init(repeat) _actimer_init(repeat)
actimer_t *_actimer_init(int repeat);
```

3.  define the macro if logic with the else part filled in.
```c
#ifdef _ACDEBUG_MEMORY_
#else
#define actimer_init(repeat) _actimer_init(repeat)
actimer_t *_actimer_init(int repeat);
#endif
```

4.  Add const char *caller to the debug version of _actimer_init
```c
actimer_t *_actimer_init(int repeat, const char *caller);
```

5.  define the macro to call the init function.
```c
#define actimer_init(repeat) _actimer_init(repeat, ACFILE_LINE_MACRO("actimer"))
```

6.  put the two calls in the #ifdef _ACDEBUG_MEMORY_ section.
```c
#ifdef _ACDEBUG_MEMORY_
#define actimer_init(repeat) _actimer_init(repeat, ACFILE_LINE_MACRO("actimer"))
actimer_t *_actimer_init(int repeat, const char *caller);
#else
#define actimer_init(repeat) _actimer_init(repeat)
actimer_t *_actimer_init(int repeat);
#endif
```

Objects will typically use the ACFILE_LINE_MACRO("object_name") when defining the init call, as in step 5 above.

Change actimer.c from
```c
actimer_t *actimer_init(int repeat) {
  actimer_t *t = (actimer_t *)malloc(sizeof(actimer_t));
```

to
```c
#ifdef _ACDEBUG_MEMORY_
actimer_t *_actimer_init(int repeat, const char *caller) {
  actimer_t *t =
    (actimer_t *)_acmalloc_d(NULL, caller,
                                   sizeof(actimer_t), false);
#else
actimer_t *_actimer_init(int repeat) {
  actimer_t *t = (actimer_t *)acmalloc(sizeof(actimer_t));
#endif
```

1.  Change all malloc, calloc, strdup, realloc, and free calls to have ac prefix.<br/>
2.  Change the function name from actimer_init to _actimer_init.<br/>
3.  Wrap the block in a #ifdef _ACDEBUG_MEMORY_/#else/#endif block<br/>
4.  Define the _ACDEBUG_MEMORY_ portion.  The _actimer_init function has the extra const char *caller parameter.  The allocation uses _acmalloc_d directly, as shown above.

To test these changes out, I've modified code from $ac/illustrations/2_timing/13_timer


The following code is found in <i>illustrations/6_allocator/1_timing</i>
```bash
cd $ac/illustrations/6_allocator/1_timing
```

```bash
$ make
gcc -O3 -I../../../src -D_ACDEBUG_MEMORY_=NULL ../../../src/actimer.c ../../../src/acallocator.c ../../../src/acbuffer.c ../../../src/acpool.c test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 7.7260ns
Reverse => esreveR
time_spent: 1.8180ns
overall time_spent: 9.5440ns
99 byte(s) allocated in 4 allocations (160 byte(s) overhead)
test_timer.c:24: 27
test_timer.c:26 [actimer]: 32
test_timer.c:24: 8
test_timer.c:26 [actimer]: 32
```

Include acallocator.h in test_timer.c
```c
#include "acallocator.h"
```

and alter the malloc and free calls to acmalloc and acfree.  I've intentionally commented out several destroy calls and the acfree call.

When make was run above, the following extra lines were output
```
99 byte(s) allocated in 4 allocations (160 byte(s) overhead)
test_timer.c:24: 27
test_timer.c:26 [actimer]: 32
test_timer.c:24: 8
test_timer.c:26 [actimer]: 32
```

This indicates that 4 allocations were not properly freed.

line 24
```c
char *s = (char *)acmalloc(len+1);
```

line 26
```c
actimer_t *copy_timer = actimer_init(actimer_get_repeat(overall_timer));
```

To fix the code, we need to make sure that copy_timer is destroyed and s is freed.  Lines 47-48 are what need uncommented to make this work.
```c
// actimer_destroy(copy_timer);
// acfree(s);
```

The error reported logged the actimer_init line as opposed to the acmalloc inside of actimer_init.  This is likely more useful unless you are testing the individual object.

Go ahead and uncomment those lines and run make again
```bash
$ make
gcc -O3 -I../../../src -D_ACDEBUG_MEMORY_=NULL ../../../src/actimer.c ../../../src/acallocator.c ../../../src/acbuffer.c ../../../src/acpool.c test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 7.7260ns
Reverse => esreveR
time_spent: 1.8180ns
overall time_spent: 9.5440ns
```

Go ahead and revert your changes.  You can revert your changes in the current directory by running the following git command.  If you don't specify the ., then it will revert all changes in the whole repo (so this command can be somewhat dangerous!)
```bash
git checkout .
```

The lines indicating memory loss are no longer printed.  In this example, _ACDEBUG_MEMORY_ was defined as NULL in the Makefile using the following line.
```Makefile
FLAGS += -D_ACDEBUG_MEMORY_=NULL
```

If we change this to
```Makefile
FLAGS += -D_ACDEBUG_MEMORY_=\"memory.log\"
```
run
```bash
make clean
```

and then
```bash
$ make
gcc -O3 -I../../../src -D_ACDEBUG_MEMORY_=\"memory.log\" ../../../src/actimer.c ../../../src/acallocator.c ../../../src/acbuffer.c ../../../src/acpool.c test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 5.9220ns
Reverse => esreveR
time_spent: 1.5720ns
overall time_spent: 7.4940ns
```

There is a new file called memory.log in this directory.
```bash
$ ls
Makefile	memory.log	test_timer	test_timer.c
```

You can view memory.log by running
```bash
$ cat memory.log
99 byte(s) allocated in 4 allocations (160 byte(s) overhead)
test_timer.c:24: 27
test_timer.c:26 [actimer]: 32
test_timer.c:24: 8
test_timer.c:26 [actimer]: 32
```

If you view the test_timer.c code, you will notice that a significant portion of the main function now has the ac prefix.

test_timer.c (main function)
```c
int main( int argc, char *argv[]) {
  int repeat_test = 1000000;
  actimer_t *overall_timer = actimer_init(repeat_test);
  for( int i=1; i<argc; i++ ) {
    size_t len = strlen(argv[i]);
    char *s = (char *)acmalloc(len+1);

    actimer_t *copy_timer = actimer_init(actimer_get_repeat(overall_timer));
    actimer_start(copy_timer);
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
    }
    actimer_stop(copy_timer);

    actimer_t *test_timer = actimer_init(actimer_get_repeat(overall_timer));
    actimer_start(test_timer);
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
      reverse_string(s, len);
    }
    actimer_stop(test_timer);
    actimer_subtract(test_timer, copy_timer);
    actimer_add(overall_timer, test_timer);

    printf("%s => %s\n", argv[i], s);
    printf( "time_spent: %0.4fns\n", actimer_ns(test_timer) );

    actimer_destroy(test_timer);
    // actimer_destroy(copy_timer);
    // acfree(s);
  }
  printf( "overall time_spent: %0.4fns\n", actimer_ns(overall_timer) );
  actimer_destroy(overall_timer);
  return 0;
}
```

Breaking out all of the ac statements:
```c
  actimer_t *overall_timer = actimer_init(repeat_test);
    char *s = (char *)acmalloc(len+1);

    actimer_t *copy_timer = actimer_init(actimer_get_repeat(overall_timer));
    actimer_start(copy_timer);

    actimer_stop(copy_timer);

    actimer_t *test_timer = actimer_init(actimer_get_repeat(overall_timer));
    actimer_start(test_timer);

    actimer_stop(test_timer);
    actimer_subtract(test_timer, copy_timer);
    actimer_add(overall_timer, test_timer);

    actimer_destroy(test_timer);
    // actimer_destroy(copy_timer);
    // acfree(s);
  actimer_destroy(overall_timer);
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

You can see that there are timer objects, malloc, and free.  Given that malloc, calloc, realloc, strdup, and free are so common in code, I opted not to provide any extra qualifiers other than ac.  I aim to make code highly optimized and very readable.  Another important feature of qualified naming is that it makes it possible to search for all places something exists.  For example, to find all cases where actimer are used, you can run..

```bash
cd $ac/illustrations
grep -rn actimer .
```  

and your output might look like...
```
./2_timing/12_timer/test_timer.c:1:#include "actimer.h"
./2_timing/12_timer/test_timer.c:20:  actimer_t *overall_timer = actimer_init(repeat_test);
./2_timing/12_timer/test_timer.c:25:    actimer_t *copy_timer = actimer_init(actimer_get_repeat(overall_timer));
./2_timing/12_timer/test_timer.c:26:    actimer_start(copy_timer);
./2_timing/12_timer/test_timer.c:30:    actimer_stop(copy_timer);
./2_timing/12_timer/test_timer.c:32:    actimer_t *test_timer = actimer_init(actimer_get_repeat(overall_timer));
./2_timing/12_timer/test_timer.c:33:    actimer_start(test_timer);
./2_timing/12_timer/test_timer.c:38:    actimer_stop(test_timer);
./2_timing/12_timer/test_timer.c:39:    actimer_subtract(test_timer, copy_timer);
./2_timing/12_timer/test_timer.c:40:    actimer_add(overall_timer, test_timer);
./2_timing/12_timer/test_timer.c:43:    printf( "time_spent: %0.4fns\n", actimer_ns(test_timer) );
./2_timing/12_timer/test_timer.c:45:    actimer_destroy(test_timer);
./2_timing/12_timer/test_timer.c:46:    actimer_destroy(copy_timer);
./2_timing/12_timer/test_timer.c:49:  printf( "overall time_spent: %0.4fns\n", actimer_ns(overall_timer) );
./2_timing/12_timer/test_timer.c:50:  actimer_destroy(overall_timer);
./2_timing/12_timer/Makefile:2:OBJECTS=$(ROOT)/src/actimer.c
./2_timing/12_timer/Makefile:3:HEADER_FILES=$(ROOT)/src/actimer.h
Binary file ./2_timing/12_timer/test_timer matches
./2_timing/13_timer/test_timer.c:1:#include "actimer.h"
./2_timing/13_timer/test_timer.c:20:  actimer_t *overall_timer = actimer_init(repeat_test);
./2_timing/13_timer/test_timer.c:25:    actimer_t *copy_timer = actimer_init(actimer_get_repeat(overall_timer));
./2_timing/13_timer/test_timer.c:26:    actimer_start(copy_timer);
./2_timing/13_timer/test_timer.c:30:    actimer_stop(copy_timer);
...
```

This feature alone is extremely valuable when working with a large codebase.  It takes longer to write each line of code, but the reader can easily find every line of code where the object is used.  This makes it easier to find example code and to build upon the work of others.

# A Quick Recap

1.  The 5 basic allocation methods in C are...
```c
void *malloc(size_t len);
void *calloc(size_t num, size_t len);
void *realloc(void *p, size_t len);
char *strdup(char *p);
void free(void *p);
```

2.  We define those functions with the ac prefix such that if we are debugging memory, we can track where the allocations are made.  This allows us to recognize memory leaks and potentially a couple of other common errors.

3.  Callback functions should have a suffix of _f

4.  We defined an approach to find when objects are created (and not destroyed).  The basic changes to actimer were outlined to make it support using the acallocator.

5.  When we are using the acallocator object outside of objects, the only change to the code is to replace the 5 basic allocation methods with `ac...` If, for some reason, you do not wish to allocate memory using the ac method, then make sure that you don't free it with the ac method.

6.  We can define _ACDEBUG_MEMORY_ as NULL and have memory leaks reported to the terminal when the program exits.

```Makefile
FLAGS += -D_ACDEBUG_MEMORY_=\"memory.log\"
```

7.  We can define _ACDEBUG_MEMORY_ as a string and have memory leaks reported to a file periodically.  The period is defined in seconds as _ACDEBUG_MEMORY_SPEED_ and defaults to 60 in accommon.h

8.  You can grep for any line of code which contains an object using the following approach.  The following grep line will search all subdirectories for the string actimer and report the filename and line number where the text is found.

```bash
grep -rn actimer .
```

actimer can be replaced with a function name or another object name (or whatever you want to find).

# Continue to build out the [allocator implementation](8_allocator_impl.md)!

[Table of Contents](README.md)  - Copyright 2019 Andy Curtis
