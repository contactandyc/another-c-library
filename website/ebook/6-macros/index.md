---
path: "/6-macros"
posttype: "tutorial"
title: "6. Macros"
---

The C compiler allows us to create a macro (basically defining one thing as another).  Two individual constants can be useful for debugging.

```
__LINE__ - The line of code that you are on.
__FILE__ - The file that the code exists in.
```

The following code is found in <i>illustrations/6\_macros/1\_macro</i>
```c
#include <stdio.h>

int main( int argc, char *argv[]) {
  printf( "This line of code is at line %d in the file %s\n", __LINE__,  __FILE__);
  return 0;
}
```

Build it and run it
```
$ make
gcc test_code.c -o test_code
./test_code
This line of code is at line 20 in the file test_code.c
```

__FILE__ is of the type const char * (meaning it is a sequence of read-only characters).   __LINE__ is an int (integer).

We can utilize the compiler and macros to help us here.  In C, you define a macro using `#define`.  

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

Before, it is turned into a binary.  The macro gets placed inline in the code rather than calling a function.  Another example is a macro referencing a variable that is defined in the main function.

```c
#include <stdio.h>

#define multiply_by_x(y) x*y

int main( int argc, char *argv[]) {
  int x = 5;
  printf("5 x 6 = %d\n", multiply_by_x(6));
  return 0;
}
```

Notice that x doesn't exist when multiply\_by\_x is defined.  Because it is replaced, it would look like the following.

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

An example of a macro calling a function.
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

Macros rarely end in a semicolon.  The following will throw an error.
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

```
$ gcc test_code.c -o test_code
test_special_constants.c:11:26: error: unexpected ';' before ')'
  printf("5 x 6 = %d\n", multiply_by_x(6));
                         ^
test_special_constants.c:3:40: note: expanded from macro 'multiply_by_x'
#define multiply_by_x(y) multiply(x, y);
                                       ^
```

The above code gets converted to
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

Notice the extra semicolon after multiply.  Generally, macros can contain semicolons but cannot end in semicolons.  Macros can also define multiple lines of code (or multiple statements).
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

```
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

The following code is found in <i>illustrations/6\_macros/2\_macro</i>
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

```
$ make
gcc test_code.c -o test_code
./test_code
before swap: (5, 10)
after swap: (10, 5)
```

One common error with multi-line macros is putting a space after the backslash.  The compiler will give you an error for doing this.  Also, I put the \ so that they all line up vertically.  This just makes the code more readable - the compiler doesn't care.  A second error with multi-line macros is to put the \ after the last line.  The \ continues code to the next line.  It's an error to put the \ on the last line (which may or may not get reported by the compiler in a useful way).

Macros can be defined in different ways depending upon another macro variable.
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

```
$ gcc test_code.c -o test_code
$ ./test_code
NOT DEBUG: 5
```

You can define compiler directives from the command line using the -D option in gcc.  Multiple directives can be defined by repeating the -D option.
```
$ gcc -D_DEBUG_ test_code.c -o test_code
$ ./test_code
DEBUG: 5
```

Finally, we can have the compiler create a single string constant out of the __FILE__ and the __LINE__ (which can include additional information).  Converting a number to a string using `#define` is a little tricky due to how the preprocessor works.  It has to be done in two passes using a function which calls a function.  The preprocessor doesn't do recursion.  Instead, it works by doing two passes.

For example
```c
#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)
#define __FILE_LINE__ __FILE__ ":" STRINGIZE(__LINE__)
#define FILE_LINE_MACRO(a) __FILE_LINE__ " [" a "]"
```

To resolve `__FILE_LINE__` (assuming we have a file named test_code.c and line 9)
```c
#include <stdio.h>

#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)
#define __FILE_LINE__ __FILE__ ":" STRINGIZE(__LINE__)
#define FILE_LINE_MACRO(a) __FILE_LINE__ " [" a "]"

int main( int argc, char *argv[]) {
  printf ("%s\n", __FILE_LINE__ );
  return 0;
}
```

The first pass will look like:
```c
#include <stdio.h>

#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)
#define __FILE_LINE__ __FILE__ ":" STRINGIZE(__LINE__)
#define FILE_LINE_MACRO(a) __FILE_LINE__ " [" a "]"

int main( int argc, char *argv[]) {
  printf ("%s\n", "test_code.c" ":" STRINGIZE2(9) );
  return 0;
}
```

The second pass will look like...
```c
#include <stdio.h>

#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)
#define __FILE_LINE__ __FILE__ ":" STRINGIZE(__LINE__)
#define FILE_LINE_MACRO(a) __FILE_LINE__ " [" a "]"

int main( int argc, char *argv[]) {
  printf ("%s\n", test_code.c" ":" "9"  );
  return 0;
}
```

In ac_common.h, I also defined AC_FILE_LINE_MACRO, which is a macro meant for objects such as the ac_timer object.  This will become more evident as we work through the allocator object.  As usual, it's a good idea to define macros using a prefix to ensure that your code doesn't conflict with other codebases.

$ac/src/ac\_common.h
```c
#ifndef _ac_common_H
#define _ac_common_H

/* defines NULL, size_t, offsetof */
#include <stddef.h>
/* because I like to use true, false, and bool */
#include <stdbool.h>

/*
Defining _AC_DEBUG_MEMORY_ will check that memory is properly
freed (and try some rudimentary double-free checks).  If memory
doesn't seem to be previously allocated, there is a scan to find
the closest block.  _AC_DEBUG_MEMORY_ can be defined as NULL or
a valid string.  If it is defined as a string, then a file will be
written with the given name every _AC_DEBUG_MEMORY_SPEED_ seconds.
Snapshots are saved in increasing intervals.
*/
// #define _AC_DEBUG_MEMORY_ "memory.log"

/* How often should the memory be checked? It is always checked in the
   beginning and every _AC_DEBUG_MEMORY_SPEED_ seconds assuming
   _AC_DEBUG_MEMORY_ is defined as a string (and not NULL). */
#define _AC_DEBUG_MEMORY_SPEED_ 60

/*
  Given an address of a member of a structure, the base object type, and the field name,
  return the address of the base structure.
*/
#define ac_parent_object(addr, base_type, field) (base_type *)((char *)addr-offsetof(base_type,field))

#define AC_STRINGIZE2(x) #x
#define AC_STRINGIZE(x) AC_STRINGIZE2(x)
#define __AC_FILE_LINE__ __FILE__ ":" AC_STRINGIZE(__LINE__)
#define AC_FILE_LINE_MACRO(a) __AC_FILE_LINE__ " [" a "]"

#endif
```

The ac\_parent\_object macro is useful for finding the address of a structure when given a pointer to a member of the structure.

```c
#include "ac_common.h"

#include <stdio.h>

typedef struct {
  int x;
  int y;
} point_t;

int main( int argc, char *argv[] ) {
  point_t point;
  point.x = 1;
  point.y = 2;
  point_t *p = ac_parent_object(&point.y, point_t, y);
  printf( "point is found at %p, p is %s\n", &point, &point == p ? "equal" : "not equal" );
  return 0;
}
```

1.  Get the address of point using the ampersand.<br/>
2.  compare that p and the address of point are the same</br>
3.  if they are the same, do what's between ? and :<br>
4.  else, do what's after the :
```c
&point == p ? "equal" : "not equal"
```

[Table of Contents (only if viewing on Github)](../../../README.md)
