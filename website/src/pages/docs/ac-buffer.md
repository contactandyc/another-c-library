---
title: ac_buffer
description:
---

```c
#include "ac_buffer.h"
```

The ac\_buffer object is much like the C++ string class in that it supports a number of easy to use string methods and a buffer acts like a string in that a buffer grows as needed to fit the contents of the string.  The object is more than a string object in as much as it also equally allows for binary data to be appended to or a combination of binary data and strings.  At it's core, it is simply a buffer that will auto-resize as needed.  The pool object is different in that it is used for lots of smaller allocations.  The buffer generally is used to track "one thing".  That one thing might consist of many parts, but they are generally all tracked together in a contiguous space.

The following files are necessary to include in your own package.  You can also just include the whole ac\_ library.

```
ac_allocator.h
ac_allocator.c
ac_pool.h
ac_pool.c
impl/ac_pool.h
ac_buffer.h
ac_buffer.c
impl/ac_buffer.h
ac_common.h
```

All of the example code is found in examples/ac\_buffer.  

## ac\_buffer\_init

```c
ac_buffer_t *ac_buffer_init(size_t size);
```
ac\_buffer\_init creates a buffer with an initial size of size.  The buffer will grow as needed, but if you know the size that is generally needed, it may be more efficient to initialize it to that size.

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setf(bh, "%s %s!", "Hello", "Buffer"); // setf
   print Hello Buffer! followed with a newline
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_pool\_init

```c
ac_buffer_t *ac_buffer_pool_init(ac_pool_t *pool, size_t size);
```
ac\_buffer\_pool\_init is like ac\_buffer\_init, except allocated with a pool and there no need to destroy it.

```c
#include "ac_buffer.h"
#include "ac_pool.h"

int main(int argc, char *argv[]) {
  ac_pool_t *pool = ac_pool_init(4096);
  ac_buffer_t *bh = ac_buffer_pool_init(pool, 10);
  ac_buffer_setf(bh, "%s %s!", "Hello", "Buffer"); // setf
   print Hello Buffer! followed with a newline
  printf("%s\n", ac_buffer_data(bh));
  // Not needed because pool used!
  // ac_buffer_destroy(bh);
  ac_pool_destroy(pool);
  return 0;
}
```

## ac\_buffer\_clear

```c
void ac_buffer_clear(ac_buffer_t *h);
```
ac\_buffer\_clear will clear the buffer.

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setf(bh, "%s %s!", "Hello", "Buffer"); // setf
   print Hello Buffer! followed with a newline
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_clear(bh);
  // notice appendf instead of setf... since after clear it will be equivalent
  ac_buffer_appendf(bh, "%s %s!", "Hello", "Again"); // setf
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_destroy

```c
void ac_buffer_destroy(ac_buffer_t *h);
```
ac\_buffer\_destroy will destroy the buffer.

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setf(bh, "%s %s!", "Hello", "Buffer"); // setf
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_data

```c
char *ac_buffer_data(ac_buffer_t *h);
```
ac\_buffer\_data gets the contents of the buffer.

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setf(bh, "%s %s!", "Hello", "Buffer"); // setf
   print Hello Buffer! followed with a newline
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_length

```c
size_t ac_buffer_length(ac_buffer_t *h);
```
ac\_buffer\_length returns the length of the buffer

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setf(bh, "%s %s!", "Hello", "Buffer"); // setf
   print Hello Buffer! followed with a newline
  printf("%lu: %s\n", ac_buffer_length(bh), ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_append

```c
void ac_buffer_append(ac_buffer_t *h, const void *data, size_t length);
```
ac\_buffer\_append appends bytes to the current buffer

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_sets(bh, "Hello");
  ac_buffer_append(bh, " Buffer!", strlen(" Buffer!")); // append
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_appends

```c
void ac_buffer_appends(ac_buffer_t *h, const char *s);
```
ac\_buffer\_appends appends a string to the current buffer

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_sets(bh, "Hello");
  ac_buffer_appends(bh, " Buffer!");
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_appendc

```c
void ac_buffer_appendc(ac_buffer_t *h, char ch);
```
ac\_buffer\_appendc appends a character to the current buffer

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setc(bh, 'H');
  ac_buffer_appendc(bh, 'e'); // appendc
  ac_buffer_appendc(bh, 'l'); // appendc
  ac_buffer_appendc(bh, 'l'); // ...
  ac_buffer_appendc(bh, 'o');
  ac_buffer_appendc(bh, ' ');
  ac_buffer_appendc(bh, 'B');
  ac_buffer_appendc(bh, 'u');
  ac_buffer_appendc(bh, 'f');
  ac_buffer_appendc(bh, 'f');
  ac_buffer_appendc(bh, 'e');
  ac_buffer_appendc(bh, 'r');
  ac_buffer_appendc(bh, '!');
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```


## ac\_buffer\_appendn

```c
void ac_buffer_appendn(ac_buffer_t *h, char ch, ssize_t n);
```
ac\_buffer\_appendn appends a character n times to the current buffer

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setc(bh, 'H');
  ac_buffer_appendc(bh, 'e');
  ac_buffer_appendn(bh, 'l', 2); // appendn
  ac_buffer_appendc(bh, 'o');
  ac_buffer_appendc(bh, ' ');
  ac_buffer_appendc(bh, 'B');
  ac_buffer_appendc(bh, 'u');
  ac_buffer_appendn(bh, 'f', 2); // appendn
  ac_buffer_appendc(bh, 'e');
  ac_buffer_appendc(bh, 'r');
  ac_buffer_appendc(bh, '!');
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_appendf

```c
void ac_buffer_appendf(ac_buffer_t *h, const char *fmt, ...);
```
ac\_buffer\_appendf appends bytes in current buffer using a formatted string -similar to printf

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setf(bh, "%s ", "Hello");
  ac_buffer_appendf(bh, "%s!", "Buffer"); // appendf
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_appendvf

```c
void ac_buffer_appendvf(ac_buffer_t *h, const char *fmt, va_list args);
```
ac\_buffer\_appendvf appends bytes in current buffer using va_args and formatted string

```c
#include "ac_buffer.h"

void my_buffer_appendf(ac_buffer_t *bh, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ac_buffer_appendvf(bh, fmt, args);
  va_end(args);
}

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setf(bh, "%s ", "Hello");
  my_buffer_appendf(bh, "%s!", "Buffer");

  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_set

```c
void ac_buffer_set(ac_buffer_t *h, const void *data, size_t length);
```
ac\_buffer\_set sets bytes to the current buffer

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_set(bh, "Hello Buffer!", strlen("Hello Buffer!")); // set
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_sets

```c
void ac_buffer_sets(ac_buffer_t *h, const char *s);
```
ac\_buffer\_sets sets a string to the current buffer

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_sets(bh, "Hello Buffer!"); // sets
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_setc

```c
void ac_buffer_setc(ac_buffer_t *h, char ch);
```
ac\_buffer\_setc sets a character to the current buffer

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setc(bh, 'H');
  /* print H followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_setn

```c
void ac_buffer_setn(ac_buffer_t *h, char ch, ssize_t n);
```
ac\_buffer\_setn sets a character n times to the current buffer

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setn(bh, 'H', 20);
  /* print HHHHHHHHHHHHHHHHHHHH followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_setf

```c
void ac_buffer_setf(ac_buffer_t *h, const char *fmt, ...);
```
ac\_buffer\_setf sets bytes in current buffer using a formatted string -similar to printf

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setf(bh, "%s %s!", "Hello", "Buffer"); // setf
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_setvf

```c
void ac_buffer_setvf(ac_buffer_t *h, const char *fmt, va_list args);
```
ac\_buffer\_setvf sets bytes in current buffer using va_args and formatted string

```c
#include "ac_buffer.h"

void my_buffer_setf(ac_buffer_t *bh, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ac_buffer_setvf(bh, fmt, args);
  va_end(args);
}

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  my_buffer_setf(bh, "%s %s!", "Hello", "Buffer"); // setf
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_resize

```c
void *ac_buffer_resize(ac_buffer_t *h, size_t length);
```
ac\_buffer\_resize resizes the buffer and return a pointer to the beginning of the buffer.  This will retain the original data in the buffer for up to length bytes.

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_sets(bh, "Hello Buffer!"); // sets
  ac_buffer_resize(bh, 5);
  /* print Hello followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_shrink\_by

```c
void *ac_buffer_shrink_by(ac_buffer_t *h, size_t length);
```
ac\_buffer\_shrink\_by shrinks the buffer by length bytes, if the buffer is not length bytes, buffer will be cleared.

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_sets(bh, "Hello Buffer!"); // sets
  ac_buffer_shrink_by(bh, 6);
  /* print Hello B followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_append\_alloc

```c
void *ac_buffer_append_alloc(ac_buffer_t *h, size_t length);
```
ac\_buffer\_append\_alloc will grow the buffer by length bytes and return pointer to the new memory.  This will retain the original data in the buffer for up to length bytes. This function will return aligned memory (and pad the end of the buffer to make the memory aligned).  This function is rarely used.  ac\_buffer\_append\_ualloc allocates without padding and may not be aligned.


## ac\_buffer\_append\_ualloc

```c
void *ac_buffer_append_ualloc(ac_buffer_t *h, size_t length);
```
ac\_buffer\_append\_ualloc is the same as ac\_buffer\_append\_alloc except memory is not necessarily aligned.

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_sets(bh, "Hello");
  ac_buffer_appendc(bh, ' ');
  char *data = (char *)ac_buffer_append_ualloc(bh, strlen("Buffer!"));
  strcpy(data, "Buffer!");
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

## ac\_buffer\_alloc

```c
void *ac_buffer_alloc(ac_buffer_t *h, size_t length);
```
ac\_buffer\_alloc will resize the buffer and return a pointer to the beginning of the buffer.  This will NOT retain the original data in the buffer for up to length bytes.

```c
#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  char *data = (char *)ac_buffer_alloc(bh, strlen("Hello Buffer!"));
  strcpy(data, "Hello Buffer!");
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```
