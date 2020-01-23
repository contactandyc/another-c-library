---
path: "/ac-allocator"
posttype: "docs"
title: "ac_allocator"
---

# ac_allocator

```c
#include "ac_allocator.h"
```

The allocator provides an alternative to malloc, calloc, realloc, strdup, and free.  The functions are prefixed with ac\_ (as everything in this library is).  

The following files are necessary to include in your own package.  You can also just include the whole ac\_ library.

```
ac_allocator.h
ac_allocator.c
ac_common.h
```

malloc, calloc, realloc, strdup, and free functions work identically to the functions found in stdlib.h and string.h without the ac\_ prefix.  

Additional documentation for [malloc,calloc,realloc,free](https://linux.die.net/man/3/malloc) and [strdup](https://linux.die.net/man/3/strdup) exists.  

Memory allocated using one of the ac\_ methods above, it must later be freed using ac\_free.  I did alter the prototype for calloc to accept a single parameter instead of 2.  I believe this makes application code more readable.   

## ac_malloc
```c
void *ac_malloc(size_t length);
```
ac\_malloc allocates length uninitialized bytes and returns it.  If length bytes is not available (the system is out of RAM or doesn't have enough to satisfy the request), NULL is returned.

```c
#include "ac_allocator.h"

#include <stdio.h>

int main( int argc, char *argv[]) {
  /* Allocate a block that is sized to fit 3 ints */
  int *x = (int *)ac_malloc(sizeof(int) * 3);
  x[0] = 1;
  x[1] = 2;
  x[2] = 3;
  printf( "%d, %d, %d\n", x[0], x[1], x[2] );
  /* Free the allocated block */
  ac_free(x);
  return 0;
}
```

## ac_calloc
```c
void *ac_calloc(size_t length);
```
ac\_calloc similar to ac\_malloc, except returned bytes are all set to zero.  I did alter the prototype for calloc to accept a single parameter instead of 2.  I believe this makes application code more readable.

```c
#include "ac_allocator.h"

#include <stdio.h>

int main( int argc, char *argv[]) {
  /* Allocate a zeroed block that is sized to fit 3 ints */
  int *x = (int *)ac_calloc(sizeof(int) * 3);
  x[0] = 1;
  // x[1] = 2;
  x[2] = 3;
  /* This should print 1, 0, 3 - zero because memory is zeroed via ac_calloc */
  printf( "%d, %d, %d\n", x[0], x[1], x[2] );
  /* Free the allocated block */
  ac_free(x);
  return 0;
}
```

## ac_realloc
```c
void *ac_realloc(void *p, size_t length);
```
ac\_realloc attempt to grow or shrink previously allocated memory to be equal to length bytes.  Typically, this will involve allocating a new block of memory and copying the contents of the previously allocated memory (up to length bytes).  It is possible that the block of memory will remain the same, but with a new size.

```c
#include "ac_allocator.h"

#include <stdio.h>

int main( int argc, char *argv[]) {
  /* Allocate a zeroed block that is sized to fit 3 ints */
  int *x = (int *)ac_malloc(sizeof(int) * 2);
  x[0] = 1;
  x[1] = 2;
  x = (int *)ac_realloc(x, sizeof(int) * 3);
  x[2] = 3;

  /* This should print 1, 2, 3 */
  printf( "%d, %d, %d\n", x[0], x[1], x[2] );
  /* Free the allocated block */
  ac_free(x);
  return 0;
}
```

## ac_memdup
```c
void *ac_memdup(void *d, size_t len);
```
ac\_memdup returns an allocated block of len bytes and duplicates d into it.

```c
#include "ac_allocator.h"

#include <stdio.h>

int main( int argc, char *argv[]) {
  /* Allocate a block that is sized to fit 3 ints */
  int *a = (int *)ac_malloc(sizeof(int) * 3);
  a[0] = 1;
  a[1] = 2;
  a[2] = 3;
  int *b = (int *)ac_memdup(a, sizeof(int) * 3);
  b[0] += 1;
  b[1] += 1;
  b[2] += 1;
  printf( "a: %d, %d, %d\n", a[0], a[1], a[2] );
  printf( "b: %d, %d, %d\n", b[0], b[1], b[2] );
  /* Free the allocated block */
  ac_free(a);
  ac_free(b);
  return 0;
}
```

## ac_strdup
```c
char *ac_strdup(const char *p);
```
ac\_strdup returns a newly allocated block of memory that has a copy of the string passed into it.  The returned copy will not necessarily be aligned.

```c
#include "ac_allocator.h"

#include <stdio.h>

void uppercase(char *s) {
  while(*s) {
    if(*s >= 'a' && *s <= 'z')
      *s = *s - 'a' + 'A';
    s++;
  }
}

int main( int argc, char *argv[]) {
  /* copy, uppercase, and print command line arguments */
  for( int i=0; i<argc; i++ ) {
    char *s = ac_strdup(argv[i]);
    uppercase(s);
    printf("%s", s);
    ac_free(s);
    if(i+1 < argc)
      printf( " " );
    else
      printf( "\n");
  }
  return 0;
}
```

## ac_strdupf
```c
char *ac_strdupf(const char *p, ...);
```
ac\_strdupf allocates a copy of the formatted string p.  The returned copy will not necessarily be aligned.

```c
#include "ac_allocator.h"

#include <stdio.h>

int main( int argc, char *argv[]) {
  int days_in_year = 365;
  int months_in_year = 12;
  char *s = ac_strdupf( "There are %d days and %d months in a year",
                        days_in_year, months_in_year);
  printf( "%s\n", s );
  ac_free(s);
  return 0;
}
```

## ac_strdupvf
```c
char *ac_strdupvf(const char *p, va_list args);
```
ac\_strdupvf similar to ac_strdupf, except that it uses va_list args.  For example, ac_strdupf is implemented using this method as follows.

```c
char *ac_strdupf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char *r = ac_strdupvf(fmt, args);
  va_end(args);
  return r;
}
```

You can implement your own strdupf like functions in a similar manner for other objects.  For example,

```c
#include "ac_allocator.h"

#include <stdio.h>

char *my_strdupf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char *r = ac_strdupvf(fmt, args);
  va_end(args);
  return r;
}

int main(int argc, char *argv[]) {
  int days_in_year = 365;
  int months_in_year = 12;
  char *s = my_strdupf("There are %d days and %d months in a year",
                       days_in_year, months_in_year);
  printf("%s\n", s);
  ac_free(s);
  return 0;
}
```

## ac_strdupa
```c
char **ac_strdupa(char **a);
```
ac\_strdupa duplicates the NULL terminated pointer array along with all of the strings.

```c
#include "ac_allocator.h"

#include <stdio.h>

void uppercase(char *s) {
  while(*s) {
    if(*s >= 'a' && *s <= 'z')
      *s = *s - 'a' + 'A';
    s++;
  }
}

int main( int argc, char *argv[]) {
  char **a = ac_split(NULL, ',', "alpha,beta,gamma");
  char **b = ac_strdupa(a);
  for( size_t i=0; a[i] != NULL; i++ ) {
    uppercase(b[i]);
    printf( "%s=>%s", a[i], b[i]);
    if(a[i+1] != NULL)
      printf( " " );
    else
      printf( "\n");    
  }
  ac_free(a);
  ac_free(b);
  return 0;
}
```


## ac_strdupa2
```c
char **ac_strdupa2(char **a);
```
ac\_strdupa2 duplicates the NULL terminated pointer array and references the original strings.

```c
#include "ac_allocator.h"

#include <stdio.h>

void uppercase(char *s) {
  while (*s) {
    if (*s >= 'a' && *s <= 'z')
      *s = *s - 'a' + 'A';
    s++;
  }
}

int main(int argc, char *argv[]) {
  char **a = ac_split(NULL, ',', "alpha,beta,gamma");
  char **b = ac_strdupa(a);
  for (size_t i = 0; a[i] != NULL; i++) {
    uppercase(b[i]);
    // notice that both sides are uppercased!
    // this is because b is an array pointing to the same thing as a.
    printf("%s=>%s", a[i], b[i]);
    if (a[i + 1] != NULL)
      printf(" ");
    else
      printf("\n");
  }
  ac_free(a);
  ac_free(b);
  return 0;
}
```


## ac_strdupan
```c
char **ac_strdupan(char **a, size_t n);
```
ac\_strdupan returns a duplicates a pointer array of length n along with all of the strings.

```c
#include "ac_allocator.h"

#include <stdio.h>

void uppercase(char *s) {
  while(*s) {
    if(*s >= 'a' && *s <= 'z')
      *s = *s - 'a' + 'A';
    s++;
  }
}

int main( int argc, char *argv[]) {
  /* copy, uppercase, and print command line arguments */
  char **s = ac_strdupan(argv, argc);
  for( int i=0; i<argc; i++ ) {
    uppercase(s[i]);
    printf("%s", s[i]);
    if(i+1 < argc)
      printf( " " );
    else
      printf( "\n");
  }
  ac_free(s);
  return 0;
}
```

## ac_split
```c
char **ac_split(size_t *num_splits, char delim, const char *s);
```

ac\_split splits a string into N pieces using delimiter.  The array that is returned will be NULL if no splits are returned, otherwise, it will be a NULL terminated list. num_splits can be NULL if the number of returning pieces is not desired.

An example where the number of splits isn't needed (NULL passed as first argument).
```c
#include "ac_allocator.h"

#include <stdio.h>

int main( int argc, char *argv[]) {
  /* Allocate a zeroed block that is sized to fit 3 ints */
  char **s = ac_split(NULL, ',', "A,B,C");
  for( size_t i=0; n[i] != NULL; i++ )
    printf( "[%lu] %s\n", i, s[i]);
  ac_free(s);
  return 0;
}
```

An example where the number of splits is needed.
```c
#include "ac_allocator.h"

#include <stdio.h>

int main( int argc, char *argv[]) {
  /* Allocate a zeroed block that is sized to fit 3 ints */
  size_t num_s = 0;
  char **s = ac_split(&num_s, ',', "A,B,C");
  for( size_t i=0; i<num_s; i++ )
    printf( "[%lu] %s\n", i, s[i]);
  ac_free(s);
  return 0;
}
```

## ac_free
```c
void ac_free(void *p);
```
ac_free frees memory allocated from functions above.



## Additional Documentation

The ac_allocator is a lightweight allocator (in debug mode) and identical to the methods which is based upon when not in debug mode.  In debug mode, allocations are tracked which allows one to easily catch memory leaks and some common errors.

Two macros control memory debugging.  If \_AC\_DEBUG\_MEMORY\_ is undefined, the ac\_ methods will work exactly like their C counterparts.  To enable memory debuging, \_AC\_DEBUG\_MEMORY\_ should be defined as NULL (-D\_AC\_DEBUG\_MEMORY\_=NULL) to write output to standard error or as a filename ('-D\_AC\_DEBUG\_MEMORY\_="memory.log"') to write output to files.  If a filename is used, \_AC\_DEBUG\_MEMORY\_SPEED\_ can be defined to an interval and the filename will serve as a base name with snapshots taken periodically.  These parameters can be compiled in or set in ac_common.h.

## Detecting Memory Loss

A common mistake that I've made is to forget to free memory that was previously allocated.  The ac_... allocation methods assist developers in making sure that allocations are cleaned up.

demo1.c
```c
#include "ac_allocator.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Demo to show how allocations are tracked\n");
  char *s = ac_strdup(argv[0]);
  printf("%s\n", s);
  // ac_free(s);
  return 0;
}
```

```
$ gcc demo1b.c ../src/ac_allocator.c -I../src -D_AC_DEBUG_MEMORY_=NULL -o demo1
$ ./demo1
Demo to show how allocations are tracked
./demo1
8 byte(s) allocated in 1 allocations (40 byte(s) overhead)
demo1.c:7: 8
```

The above example, allocates 8 bytes (demo1.c is 7 bytes + the \0 terminator).  When demo1 exits, it shows how many allocations have not been freed and each filename:line along with the number of bytes allocated (and not freed).  Modifying demo1.c by uncommenting the ac_free call should eliminate the memory leak.

demo1.c
```c
#include "ac_allocator.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Demo to show how allocations are tracked\n");
  char *s = ac_strdup(argv[0]);
  printf("%s\n", s);
  ac_free(s);
  return 0;
}
```


```
$ gcc demo1.c ac_allocator.c -D_AC_DEBUG_MEMORY_=NULL -o demo1
$ ./demo1
Demo to show how allocations are tracked
./demo1
```

With the memory leak fixed, nothing is printed to stderr.

## Double Free

A common mistake I've made many times is to free memory twice resulting in a corrupt state and possibly crashing the program.  The ac_... allocation methods will detect freeing memory more than once.

demo1b.c
```c
#include "ac_allocator.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Demo to show double free\n");
  char *s = ac_strdup(argv[0]);
  // printf("%s\n", s);
  ac_free(s);
  ac_free(s);
  return 0;
}
```

```
$ ./demo1b
Demo to show how allocations are tracked
demo1b.c:10: 0 ac_free is invalid (double free?)
Abort trap: 6
```

Line 10 of demo1b.c frees the memory a second time.

## Freeing the Wrong Memory

malloc, calloc, realloc, and strdup all return pointers.  Those pointers must be retained to be freed later.  If you alter the pointer (by adding or subtracting from it) and then call free with the altered pointer, your program will probably crash.  The ac_... functions will attempt to find the memory meant to be freed.

demo1c.c
```c
#include "ac_allocator.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Demo to show invalid free\n");
  char *s = ac_strdup(argv[0]);
  s += 2;
  ac_free(s);
  return 0;
}
```

```
$ ./demo1c
Demo to show invalid free
demo1c.c:7: 9 is closest allocation and is 2 bytes ahead of original allocation
demo1c.c:9: 0 ac_free is invalid (double free?)
Abort trap: 6
```

The ac_free call is called with a pointer that is advanced 2 bytes beyond the original allocation at line 7 of demo1c.c.

## Tracking Memory Loss Over Time

We could alternatively define \_AC\_DEBUG\_MEMORY\_ as "memory.log" and set the refresh speed to 5 second with another example.

demo1b.c
```c
#include "ac_allocator.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("Demo to show how allocations are tracked\n");
  int tests = 32;
  int speed = 10;
  char **a = (char **)ac_malloc(sizeof(char *) * tests);
  for (int i = 0; i < tests; i++) {
    a[i] = ac_malloc(i + 1);
    sleep(speed);
  }
  printf("Cleaning up - intentionally leaving a small leak\n");
  for (int i = 0; i < tests; i++)
    ac_free(a[i]);
  // ac_free(a);
  return 0;
}
```

```
$ gcc demo1b.c ../src/ac_allocator.c -I../src -o demo1b '-D_AC_DEBUG_MEMORY_="memory.log"' -D_AC_DEBUG_MEMORY_SPEED_=5
$ ./demo1b
Demo to show how allocations are tracked
Cleaning up - intentionally leaving a small leak
$ ls -lT memory.log*
104 Dec 30 08:50:04 2019 memory.log
642 Dec 30 08:49:54 2019 memory.log.1
625 Dec 30 08:49:44 2019 memory.log.2
591 Dec 30 08:49:24 2019 memory.log.3
523 Dec 30 08:48:44 2019 memory.log.4
386 Dec 30 08:45:06 2019 memory.log.5
 25 Dec 30 08:42:25 2019 memory.log.6
```

Every 5 seconds a snapshot was taken.  Those snapshots were rotated with some of them being deleted.  This is generally meant to view the difference between what is happening now with some relatively recent event.  For example, you can see what the difference is between memory.log.1 and memory.log.2.  Note that there is a 10 second gap between them.

```
$ diff memory.log.1 memory.log.2
$ diff memory.log.1 memory.log.2
1,2c1,2
< Mon Dec 30 08:49:54 2019
< 784 byte(s) allocated in 33 allocations (1320 byte(s) overhead)
---
> Mon Dec 30 08:49:44 2019
> 752 byte(s) allocated in 32 allocations (1280 byte(s) overhead)
35d34
< demo1b.c:13: 32
```

From above, it can be seen that memory.log.1 has one extra allocation for 32 bytes at line 13 of demo1b.c.

When the program exits, it will write a final memory.log file.

```
$ cat memory.log
Mon Dec 30 08:50:04 2019
256 byte(s) allocated in 1 allocations (40 byte(s) overhead)
demo1b.c:11: 256
```

The array of pointers (256 bytes) is not freed at line 11 in demo1b.c.  Uncommenting ac_free(a); just before the return 0; would have fixed this.

## Advanced Usage

The ac_... objects have been implemented to allow tracking of the allocation of the objects themselves.  If this functionality didn't exist, the allocation errors would show up in the object instead of where the object was created.

demo2.c
```c
#include "ac_pool.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Demo to show how objects are tracked\n");
  ac_pool_t *pool = ac_pool_init(1024);
  char *s = ac_pool_strdup(pool, argv[0]);
  // printf("%s\n", s);
  // ac_pool_destroy(pool);
  return 0;
}
```

```
$ make demo2
$ ./demo2
Demo to show how objects are tracked
1024 byte(s) allocated in 1 allocations (40 byte(s) overhead)
demo2.c:7 [ac_pool] size: 8, max_size: 8, initial_size: 1024 used: 1112
```

Line 7 of demo2.c
```c
  ac_pool_t *pool = ac_pool_init(1024);
```

This is the ac_pool_init method which is expected to be destroyed at some later point via ac_pool_destroy.  If the object tracking didn't exist, the error would have reported something like..

```
1024 byte(s) allocated in 1 allocations (40 byte(s) overhead)
pool.c:65: 1024
```

Hopefully, it is clear that the first error indicating line 7 of demo2.c is much more informative than the second.  This is explained in greater detail in [The Global Allocator Object](../../docs/7-allocator/index.md) or in the [ac_pool usage guide](ac_pool/index.md).
