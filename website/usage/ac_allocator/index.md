# ac_allocator

The allocator provides an alternative to malloc, calloc, realloc, strdup, and free.  The functions are prefixed with ac_ (as everything in this library is).  

```c
#include "ac_allocator.h"

void *ac_malloc(size_t length);
void *ac_calloc(size_t length);
void *ac_realloc(void *p, size_t length);
char *ac_strdup(const char *p);
void ac_free(void *p);
```

## Dependencies
Dependencies are the files necessary to include in your own package.  You can also just include the whole ac_ library.
```
ac_allocator.h
ac_allocator.c
ac_common.h
```

## Documentation

These functions work identically to the functions found in stdlib.h and string.h without the ac_ prefix.  

void \*ac_malloc(size_t length);<br/>
Allocates length uninitialized bytes and returns it.  If length bytes is not available (the system is out of RAM or doesn't have enough to satisfy the request), NULL is returned.

void \*ac_calloc(size_t length);<br/>
Similar to ac_malloc, except returned bytes are all set to zero.

void \*ac_realloc(void \*p, size_t length);<br/>
Attempt to grow or shrink previously allocated memory to be equal to length bytes.  Typically, this will involve allocating a new block of memory and copying the contents of the previously allocated memory (up to length bytes).  It is possible that the block of memory will remain the same, but with a new size.

char \*ac_strdup(const char \*p);<br/>
Returns a newly allocated block of memory that has a copy of the string passed into it.

void ac_free(void \*p);<br/>
Free memory allocated using ac_malloc, ac_calloc, ac_realloc, and ac_strdup.

Additional documentation for [malloc,calloc,realloc,free](https://linux.die.net/man/3/malloc) and [strdup](https://linux.die.net/man/3/strdup) exists.  

If memory is allocated using one of the ac_ methods above, it must later be freed using ac_free.  I did alter the prototype for calloc to accept a single parameter instead of 2.  I believe this makes application code more readable.   

## Detecting Memory Errors

The ac_allocator is a lightweight allocator (in debug mode) and identical to the methods which is based upon when not in debug mode.  In debug mode, allocations are tracked which allows one to easily catch memory leaks and some common errors.

Two macros control memory debugging.  If \_AC_DEBUG_MEMORY_
is undefined, the ac_ methods will work exactly like their C counterparts.  To enable memory debuging, \_AC_DEBUG_MEMORY_ should be defined as NULL (-D_AC_DEBUG_MEMORY_=NULL) to write output to standard error or as a filename ('-D_AC_DEBUG_MEMORY_="memory.log"') to write output to files.  If a filename is used, \_AC_DEBUG_MEMORY_SPEED_ can be defined to an interval and the filename will serve as a base name with snapshots taken periodically.  These parameters can be compiled in or set in ac_common.h.

# Detecting Memory Loss

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

We could alternatively define \_AC_DEBUG_MEMORY_ as "memory.log" and set the refresh speed to 5 second with another example.

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
