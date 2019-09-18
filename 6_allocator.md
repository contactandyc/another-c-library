# The Allocator
## A global allocation strategy

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
#ifndef _stla_allocator_H
#define _stla_allocator_H

#include "stla_common.h"

void *stla_malloc(size_t len);
void *stla_calloc(size_t num, size_t block_size);
void *stla_realloc(void *p, size_t len);
char *stla_strdup(char *p);
void stla_free(void *p);

#endif
```

This is what the API should look like, but we can do a little better.  These functions will all be called from a line of code which will be in a file.  The c compiler allows us to create a macro (basically defining one thing as another).  There are two special constants that can be useful for debugging.

```
__LINE__ - The line of code that you are on.
__FILE__ - The file that the code exists in.
```

For example if you were to create this
test_special_constants.c
```c
#include <stdio.h>

int main( int argc, char *argv[]) {
  printf( "This line of code is at line %d in the file %s\n", __LINE__,  __FILE__);
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

__FILE__ is of the type const char * (meaning it is a sequence of read only characters).   __LINE__ is an int (integer).

We could redefine our interface as

```c
#ifndef _stla_allocator_H
#define _stla_allocator_H

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
#include "stla_allocator.h"

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

Finally, we can have the compiler create a single string constant out of the __FILE__ and the __LINE__ (which can include additional information).  Converting a number to a string using #define is a little tricky due to how the preprocessor works.  It basically has to be done in two passes using a function which calls a function.  The proprocessor doesn't do recursion.  Instead, it works by doing two passes.

$stla/src/stla_common.h
```c
#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)
#define __FILE_LINE__ __FILE__ ":" STRINGIZE(__LINE__)
#define FILE_LINE_MACRO(a) __FILE_LINE__ " [" a "]"
```

To resolve, __FILE_LINE__ (assuming we have a file named test_code.c and line 5)

test_code.c
```c
#include <stdio.h>
#include "stla_common.h"

int main( int argc, char *argv[]) {
  printf ("%s\n", __FILE_LINE__ );
  return 0;
}
```

The first pass will look like...
```c
#include <stdio.h>
#include "stla_common.h"

int main( int argc, char *argv[]) {
  printf ("%s\n", "test_code.c" ":" STRINGIZE2(5) );
  return 0;
}
```

The second pass will look like...

```c
#include <stdio.h>
#include "stla_common.h"

int main( int argc, char *argv[]) {
  printf ("%s\n", test_code.c" ":" "5"  );
  return 0;
}
```

In stla_common.h, I also defined FILE_LINE_MACRO which is a macro meant for objects such as the stla_timer object.  This will become more evident as we work through the allocator object.


In creating the global allocator, maybe we only want line numbers and malloc to be passed in when the software is defined as being in debug mode.  To define stla_malloc, we might want to do the following.  Considering that we have just identified how to merge the __FILE__ and __LINE__, we will use that.

```c
#ifdef _STLA_DEBUG_MEMORY_
#define stla_malloc(len) _stla_malloc_d(__FILE_LINE__, len)
#else
#define stla_malloc(len) malloc(len)
#endif

void *_stla_malloc_d( const char *caller, size_t len );
```

When defining functions this way, it is good to come up with a convention.  My convention is to use an underscore before the function and to suffix the debug function with _d.  In defining the final interface, I came up with a few additional features.  The first is based upon the idea that the allocator will use an internal object to represent its structure.  That object will be global, and all of the allocation functions will have to pass a pointer to the object.

```c
void *_stla_malloc_d( const char *caller, size_t len );
```

becomes
```c
void *_stla_malloc_d( stla_allocator_t *a, const char *caller, size_t len );
```

The macro changes to.
```c
#define stla_malloc(len) _stla_malloc_d(NULL, __FILE_LINE__, len)
```

A NULL allocator simply means to use the global allocator.

The second feature is to allow for a mechanism to allow for custom content in place of the caller.  For example, if we allocated a buffer and keep changing its size, we might want to know where the buffer was initialized at, the maximum size of the buffer, its initial size, etc.  This custom feature can be denoted by passing a boolean at the end which defaults to false.

```c
void *_stla_malloc_d( stla_allocator_t *a, const char *caller, size_t len );
```

becomes
```c
void *_stla_malloc_d( stla_allocator_t *a, const char *caller, size_t len, bool custom );
```

The macro changes to.
```c
#define stla_malloc(len) _stla_malloc_d(NULL, __FILE_LINE__, len, false)
```

If the custom feature is enabled, the objects need a way to dump their state to a file (or to the terminal).  In C, you can define a function pointer and then associate the pointer to functions programatically.  An example is below.  The gist of it is that you specify the new function pointer type by surrounding the name in parenthesis and an extra asterisk.

For example,
```c
typedef void (*my_function)();
```

would declare a function pointer type named my_function which took no arguments and didn't return anything (it has a void return type).  I recommend using a suffix for function pointers (I'm going to use _f).  The allocator needs to define a function pointer to allow other objects to dump their details.

```c
typedef void (*stla_dump_details_f)(FILE *out, void *p, size_t length);
```

In C, the order of the members of a structure is also the byte order of the structure.  This is often exploited to allow a structure to be able to be cast as a structure of a different type.

```c
typedef struct {
  stla_dump_details_f dump;
  const char *caller;
} stla_allocator_dump_t;
```

Another useful feature is to have the state of the memory usage dumped every so often.  All this means is that if _STLA_DEBUG_MEMORY_ is defined as a filename, any program which uses the allocator will record all of the memory allocations every N seconds and rotate the previous snapshot.  In order to maintain a smaller number of output files, the rotations will rotate files with an ever expanding gap between them.

The full allocator interface is below.

$stla/src/stla_allocator.h
```c
#ifndef _stla_allocator_H
#define _stla_allocator_H

#include "stla_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _STLA_DEBUG_MEMORY_
#define stla_malloc(len) _stla_malloc_d(NULL, __FILE_LINE__, len, false)
#define stla_calloc(len) _stla_calloc_d(NULL, __FILE_LINE__, len, false)
#define stla_realloc(p, len) _stla_realloc_d(NULL, __FILE_LINE__, p, len, false)
#define stla_strdup(p) _stla_strdup_d(NULL, __FILE_LINE__, p)
#define stla_free(p) _stla_free_d(NULL, __FILE_LINE__, p)
#else
#define stla_malloc(len) malloc(len)
#define stla_calloc(len) calloc(1, len)
#define stla_realloc(p, len) realloc(p, len)
#define stla_strdup(p) strdup(p)
#define stla_free(p) free(p)
#endif

typedef void (*stla_dump_details_f)(FILE *out, void *p, size_t length);

typedef struct {
  stla_dump_details_f dump;
  const char *caller;
} stla_allocator_dump_t;

struct stla_allocator_s;
typedef struct stla_allocator_s stla_allocator_t;

stla_allocator_t *stla_allocator_init();
void stla_allocator_destroy(stla_allocator_t *a);

void stla_dump_global_allocations(stla_allocator_t *a, FILE *out);

void *_stla_malloc_d(stla_allocator_t *a, const char *caller, size_t len, bool custom );

void *_stla_calloc_d(stla_allocator_t *a, const char *caller, size_t len, bool custom );

void *_stla_realloc_d(stla_allocator_t *a, const char *caller, void *p, size_t len, bool custom);

char *_stla_strdup_d(stla_allocator_t *a, const char *caller, const char *p );

void _stla_free_d(stla_allocator_t *a, const char *caller, void *p);

#endif
```


$stla/src/stla_common.h
```c
#ifndef _stla_common_H
#define _stla_common_H

/* defines NULL, size_t, offsetof */
#include <stddef.h>
/* because I like to use true, false, and bool */
#include <stdbool.h>

/*
Defining _STLA_DEBUG_MEMORY_ will check that memory is properly
freed (and try some rudimentary double free checks).  If memory
doesn't seem to be previously allocated, there is a scan to find
the closest block.  _STLA_DEBUG_MEMORY_ can be defined as NULL or
a valid string.  If it is defined as a string, then a file will be
written with the given name every _STLA_DEBUG_MEMORY_SPEED_ seconds.
Snapshots are saved in increasing intervals.  
*/
// #define _STLA_DEBUG_MEMORY_ "memory.log"

/* How often should the memory be checked? It is always checked in the
   beginning and every _STLA_DEBUG_MEMORY_SPEED_ seconds assuming
   _STLA_DEBUG_MEMORY_ is defined as a string (and not NULL). */
#define _STLA_DEBUG_MEMORY_SPEED_ 60


#define stla_parent_object(addr, base_type, field) (base_type *)((char *)addr-offsetof(base_type,field))

#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)
#define __FILE_LINE__ __FILE__ ":" STRINGIZE(__LINE__)
#define FILE_LINE_MACRO(a) __FILE_LINE__ " [" a "]"

#endif
```

Before understanding the implementation, let's see how this get's used by other objects and code.  The stla_timer.h/c will change in the following way.

Include stla_common.h

stla_timer.h
```c
#include "stla_common.h"
```

Replace
```c
stla_timer_t *stla_timer_init(int repeat);
```

with
```c
#ifdef _STLA_DEBUG_MEMORY_
#define stla_timer_init(repeat) _stla_timer_init(repeat, FILE_LINE_MACRO("stla_timer"))
stla_timer_t *_stla_timer_init(int repeat, const char *caller);
#else
#define stla_timer_init(repeat) _stla_timer_init(repeat)
stla_timer_t *_stla_timer_init(int repeat);
#endif
```

The above code has two basic cases.  One where _STLA_DEBUG_MEMORY_ is defined and the other where it is not (#else).  It may be easier to break this into a couple of steps.

1.  convert the init function to be prefixed with an underscore

```c
stla_timer_t *stla_timer_init(int repeat);
```

becomes
```c
stla_timer_t *_stla_timer_init(int repeat);
```

2.  create a macro which defines stla_timer_init as _stla_timer_init
```c
#define stla_timer_init(repeat) _stla_timer_init(repeat)
stla_timer_t *_stla_timer_init(int repeat);
```

3.  define the macro if logic with the else part filled in.
```c
#ifdef _STLA_DEBUG_MEMORY_
#else
#define stla_timer_init(repeat) _stla_timer_init(repeat)
stla_timer_t *_stla_timer_init(int repeat);
#endif
```

4.  Add const char *caller to the debug version of _stla_timer_init
```c
stla_timer_t *_stla_timer_init(int repeat, const char *caller);
```

5.  define the macro to call the init function.
```c
#define stla_timer_init(repeat) _stla_timer_init(repeat, FILE_LINE_MACRO("stla_timer"))
```

6.  put the two calls in the #ifdef _STLA_DEBUG_MEMORY_ section.
```c
#ifdef _STLA_DEBUG_MEMORY_
#define stla_timer_init(repeat) _stla_timer_init(repeat, FILE_LINE_MACRO("stla_timer"))
stla_timer_t *_stla_timer_init(int repeat, const char *caller);
#else
#define stla_timer_init(repeat) _stla_timer_init(repeat)
stla_timer_t *_stla_timer_init(int repeat);
#endif
```

Objects will typically use the FILE_LINE_MACRO("object_name") when defining the init call as in step 5 above.

Change stla_timer.c from
```c
stla_timer_t *stla_timer_init(int repeat) {
  stla_timer_t *t = (stla_timer_t *)malloc(sizeof(stla_timer_t));
```

to
```c
#ifdef _STLA_DEBUG_MEMORY_
stla_timer_t *_stla_timer_init(int repeat, const char *caller) {
  stla_timer_t *t =
    (stla_timer_t *)_stla_malloc_d(NULL, caller,
                                   sizeof(stla_timer_t), false);
#else
stla_timer_t *_stla_timer_init(int repeat) {
  stla_timer_t *t = (stla_timer_t *)stla_malloc(sizeof(stla_timer_t));
#endif
```

1.  Change all malloc, calloc, strdup, realloc, and free calls to have stla_ prefix.<br/>
2.  Change the function name from stla_timer_init to _stla_timer_init.<br/>
3.  Wrap the block in a #ifdef _STLA_DEBUG_MEMORY_/#else/#endif block<br/>
4.  Define the _STLA_DEBUG_MEMORY_ portion.  The _stla_timer_init function has the extra const char *caller parameter.  The allocation use _stla_malloc_d directly as shown above.

To test these changes out, I've modified code from $stla/illustrations/2_timing/13_timer


The following code is found in <i>illustrations/6_allocator/1_timing</i>
```bash
cd $stla/illustrations/6_allocator/1_timing
```

```bash
$ make
gcc -O3 -I../../../src -D_STLA_DEBUG_MEMORY_=NULL ../../../src/stla_timer.c ../../../src/stla_allocator.c ../../../src/stla_buffer.c ../../../src/stla_pool.c test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 7.7260ns
Reverse => esreveR
time_spent: 1.8180ns
overall time_spent: 9.5440ns
99 byte(s) allocated in 4 allocations (160 byte(s) overhead)
test_timer.c:24: 27
test_timer.c:26 [stla_timer]: 32
test_timer.c:24: 8
test_timer.c:26 [stla_timer]: 32
```

Include stla_allocator.h in test_timer.c
```c
#include "stla_allocator.h"
```

and alter the malloc and free calls to stla_malloc and stla_free.  I've intentionally commented out several destroy calls and the stla_free call.

When make was run above, the following extra lines were output
```
99 byte(s) allocated in 4 allocations (160 byte(s) overhead)
test_timer.c:24: 27
test_timer.c:26 [stla_timer]: 32
test_timer.c:24: 8
test_timer.c:26 [stla_timer]: 32
```

This indicates that there were 4 allocations that were not properly freed.

line 24
```c
char *s = (char *)stla_malloc(len+1);
```

line 26
```c
stla_timer_t *copy_timer = stla_timer_init(stla_timer_get_repeat(overall_timer));
```

To fix the code, we need to make sure that copy_timer is destroyed and s is freed.  Lines 47-48 are what need uncommented to make this work.
```c
// stla_timer_destroy(copy_timer);
// stla_free(s);
```

Go ahead and uncomment those lines and run make again
```bash
$ make
gcc -O3 -I../../../src -D_STLA_DEBUG_MEMORY_=NULL ../../../src/stla_timer.c ../../../src/stla_allocator.c ../../../src/stla_buffer.c ../../../src/stla_pool.c test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 7.7260ns
Reverse => esreveR
time_spent: 1.8180ns
overall time_spent: 9.5440ns
```
