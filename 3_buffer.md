[Table of Contents](README.md)  - Copyright 2019 Andy Curtis

# The Buffer Object

hello_buffer.c
```c
#include "buffer.h"

int main( int argc, char *argv[] ) {
  buffer_t *bh = buffer_init(10);
  buffer_sets(bh, "Hello");
  buffer_appendc(bh, ' ');
  buffer_appendf(bh, "%s!", "Buffer");
  /* print Hello Buffer! followed with a newline */
  printf( "%s\n", buffer_data(bh) );
  buffer_destroy(bh);
  return 0;
}
```

building...

```bash
git clone https://github.com/contactandyc/realtime.git
cd illustrations/buffer/1_buffer
gcc hello_buffer.c buffer.c -o hello_buffer
```

output
```bash
$ ./hello_buffer
Hello Buffer!
$
```

Instead of `gcc hello ... `, you can run
```bash
make
```

`make` builds and runs hello_buffer along with a few more example programs.  If `git`, `gcc`, or `make` fail, you may need to install them.  Refer to your favorite search engine and find a tutorial on how to install them on your platform.

In other languages, the code might look like

Python
```python
bh = "Hello"
bh += " "
bh += "{0}!".format("Buffer")
print(bh)
```

Javascript
```javascript
var bh = "Hello";
bh += " ";
var buffer = "Buffer";
bh += buffer + "!";
console.log(bh);
```

C++
```c++
#include <string>
#include <iostream>

int main(int argc, char *argv[]) {
  std::string s = string("");
  s = std::string("Hello");
  s += std::string(" ");
  s += std::string("Buffer") + std::string("!");
  std::cout << s << std::endl;
  return 0;
}
```

and so on...

`buffer.h` is part of this project (I'll rename `buffer.h` to `stla_buffer.h` later on).  stla stands for standard template library alternative.  C doesn't have a built-in mechanism to handle growing strings or arrays.  Therefore, every C programmer builds an object similar to buffer or reuses somebody else's.  I'm just attempting to develop standardized objects which others can use.

## A Bit of History and Setup

To build robust C code, you need to create reusable objects.  Those objects will then often be used to create larger objects.  The buffer has been one of the most used objects throughout my career.  In other languages, it would generally replace the string and the array.  This object changes in its final form as this object ultimately builds upon another object which I've called <b>pool</b> that is also one of my goto objects. I describe pool after the linked structures section.  This object plays a vital role in printing binary search trees (described in the linked structures section).  

`buffer.h` holds the interface for buffer.  This interface must be included for the C compiler to be able to prepare the code to be converted to a binary properly.  `buffer.c` and the example above contain the implementation for buffer included in the gcc compilation.  It isn't my goal to explain C in this book, but I cover some of the language aspects as I go.

```c
#include "buffer.h"
```

Every good C program that is capable of being executed has the main function.  The main function typically implements the following two parameters passed in via the command-line:
- `argc` represents the number of arguments (the name of the program is the 1st argument).  `argv `represents the arguments.  
- `argv[0]` references the name of the program as called from the command line.  The main function returns an integer.  
If the program executes successfully, it should return 0.
```c
int main( int argc, char *argv[] ) {
  ...
  return 0;
}
```

Most of the objects created throughout this book are named `object_name_t`.  Functions that pertain to the object are named `object_name_function_name`.  Many of the objects can be created through `object_name_init` and destroyed using `object_name_destroy`.  Sometimes alternate init functions and (less frequently) alternate destroy functions exist.  In general, if you init an object, you are expected to destroy the object later.  Objects will not automatically disappear unless the pool is used (discussed later).

The init function expects that you provide a hint as to how much memory your application might need.  If you specify too many bytes, your program consumes extra memory needlessly.  If you specify too small of a number, the buffer object automatically grows as needed.
```c
buffer_t *bh = buffer_init(10);
...
buffer_destroy(bh);
```

Set the buffer (bh) to have the string "Hello"
```c
buffer_sets(bh, "Hello");
```

Append a space to the buffer (bh)
```c
buffer_appendc(bh, ' ');
```

Append a formatted string where %s gets replaced with the word "Buffer" to the buffer (bh)
```c
buffer_appendf(bh, "%s!", "Buffer");
```

`/*` defines the beginning of a multi-line comment while `*/` closes the multi-line comment. `//` defines single-line comments.
```c
/* print Hello Buffer! followed with a newline */
```

Print the contents of the buffer (bh) by using the `buffer_data(bh)` to access its contents.
```c
printf( "%s\n", buffer_data(bh) );
```

## The Buffer Interface

Buffer defines several other functions described shortly.  Here is the full interface used for this tutorial:

```c
#ifndef _buffer_H
#define _buffer_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

struct buffer_s;
typedef struct buffer_s buffer_t;

/* initial size of the buffer, buffer will grow as needed */
buffer_t *buffer_init(size_t initial_size);

/* clear the buffer */
void buffer_clear(buffer_t *h);

/* get the contents of the buffer */
char *buffer_data(buffer_t *h);
/* get the length of the buffer */
size_t buffer_length(buffer_t *h);

/* append bytes to the current buffer */
void buffer_append(buffer_t *h, const void *data, size_t length);
/* append a string to the current buffer */
void buffer_appends(buffer_t *h, const char *s);
/* append a character to the current buffer */
void buffer_appendc(buffer_t *h, char ch);
/* append a character n times to the current buffer */
void buffer_appendn(buffer_t *h, char ch, ssize_t n);
/* append bytes in current buffer using va_args and formatted string */
void buffer_appendvf(buffer_t *h, const char *fmt, va_list args);
/* append bytes in current buffer using a formatted string -similar to printf */
void buffer_appendf(buffer_t *h, const char *fmt, ...);

/* set bytes to the current buffer */
void buffer_set(buffer_t *h, const void *data, size_t length);
/* set a string to the current buffer */
void buffer_sets(buffer_t *h, const char *s);
/* set a character to the current buffer */
void buffer_setc(buffer_t *h, char ch);
/* set a character n times to the current buffer */
void buffer_setn(buffer_t *h, char ch, ssize_t n);
/* set bytes in current buffer using va_args and formatted string */
void buffer_setvf(buffer_t *h, const char *fmt, va_list args);
/* set bytes in current buffer using a formatted string -similar to printf */
void buffer_setf(buffer_t *h, const char *fmt, ...);

/* resize the buffer and return a pointer to the beginning of the buffer.  This
   retains the original data in the buffer for up to length bytes. */
void *buffer_resize(buffer_t *h, size_t length);
/* grow the buffer by length bytes and return pointer to the new memory.  This
   retains the original data in the buffer for up to length bytes. */
void *buffer_append_alloc(buffer_t *h, size_t length);
/* resize the buffer and return a pointer to the beginning of the buffer.  This
   does NOT retain the original data in the buffer for up to length bytes. */
void *buffer_alloc(buffer_t *h, size_t length);

/* destroy the buffer */
void buffer_destroy(buffer_t *h);

#endif
```

One thing that you may notice is that there aren't any details as to how the buffer implements into the interface.  It is essential to separate interfaces from implementations.  If you do this, you continually improve upon the interface.  If hardware changes over time, you can reimplement the implementation part as needed without affecting the rest of the codebases.  Getting interfaces right is somewhat of an art.  There is a balance between getting specific work done and creating reusable objects. My coding style allows every line where a given object exists with `grep`.  I'm not alone in using this coding style.  Projects like `libcurl`, `libuv` (behind Google Chrome), and others use similar approaches.

## The Implementation

buffer.c
```c
#include "buffer.h"

#include <stdlib.h>
#include <string.h>

struct buffer_s {
  char *data;
  size_t length;
  size_t size;
};

buffer_t *buffer_init(size_t initial_size) {
  buffer_t *h = (buffer_t *)malloc(sizeof(buffer_t));
  h->data = (char *)malloc(initial_size + 1);
  h->data[0] = 0;
  h->length = 0;
  h->size = initial_size;
  return h;
}

void buffer_clear(buffer_t *h) {
  h->length = 0;
  h->data[0] = 0;
}

void buffer_destroy(buffer_t *h) {
  free(h->data);
  free(h);
}

char *buffer_data(buffer_t *h) { return h->data; }
size_t buffer_length(buffer_t *h) { return h->length; }

static inline void _buffer_grow(buffer_t *h, size_t length) {
  size_t len = (length + 50) + (h->size >> 3);
  char *data = (char *)malloc(len + 1);
  memcpy(data, h->data, h->length + 1);
  free(h->data);
  h->data = data;
  h->size = len;
}

void *buffer_resize(buffer_t *h, size_t length) {
  if (length > h->size)
    _buffer_grow(h, length);
  h->length = length;
  h->data[h->length] = 0;
  return h->data;
}

void *buffer_append_alloc(buffer_t *h, size_t length) {
  if (length + h->length > h->size)
    _buffer_grow(h, length + h->length);
  char *r = h->data + h->length;
  h->length += length;
  r[h->length] = 0;
  return r;
}

static inline void _buffer_append(buffer_t *h, const void *data,
                                  size_t length) {
  if (h->length + length > h->size)
    _buffer_grow(h, h->length + length);

  memcpy(h->data + h->length, data, length);
  h->length += length;
  h->data[h->length] = 0;
}

void buffer_append(buffer_t *h, const void *data, size_t length) {
  _buffer_append(h, data, length);
}

void buffer_appends(buffer_t *h, const char *s) {
  _buffer_append(h, s, strlen(s));
}

void buffer_appendc(buffer_t *h, char ch) {
  if (h->length + 1 > h->size)
    _buffer_grow(h, h->length + 1);

  char *d = h->data + h->length;
  *d++ = ch;
  *d = 0;
  h->length++;
}

void buffer_appendn(buffer_t *h, char ch, ssize_t n) {
  if (n <= 0)
    return;

  if (h->length + n > h->size)
    _buffer_grow(h, h->length + n);

  char *d = h->data + h->length;
  memset(d, ch, n);
  d += n;
  *d = 0;
  h->length += n;
}

static inline void _buffer_alloc(buffer_t *h, size_t length) {
  size_t len = (length + 50) + (h->size >> 3);
  free(h->data);
  h->data = (char *)malloc(len + 1);
  h->size = len;
}

void *buffer_alloc(buffer_t *h, size_t length) {
  if (length > h->size)
    _buffer_alloc(h, length);
  h->length = length;
  h->data[h->length] = 0;
  return h->data;
}

static inline void _buffer_set(buffer_t *h, const void *data, size_t length) {
  if (length > h->size)
    _buffer_alloc(h, length);
  memcpy(h->data, data, length);
  h->length = length;
  h->data[length] = 0;
}

void buffer_set(buffer_t *h, const void *data, size_t length) {
  _buffer_set(h, data, length);
}

void buffer_sets(buffer_t *h, const char *s) { _buffer_set(h, s, strlen(s)); }

void buffer_setc(buffer_t *h, char c) { _buffer_set(h, &c, 1); }

void buffer_setn(buffer_t *h, char ch, ssize_t n) {
  h->length = 0;
  buffer_appendn(h, ch, n);
}

void buffer_setvf(buffer_t *h, const char *fmt, va_list args) {
  h->length = 0;
  buffer_appendvf(h, fmt, args);
}

void buffer_setf(buffer_t *h, const char *fmt, ...) {
  h->length = 0;
  va_list args;
  va_start(args, fmt);
  buffer_appendvf(h, fmt, args);
  va_end(args);
}

void buffer_appendvf(buffer_t *h, const char *fmt, va_list args) {
  va_list args_copy;
  va_copy(args_copy, args);
  size_t leftover = h->size - h->length;
  char *r = h->data + h->length;
  int n = vsnprintf(r, leftover, fmt, args_copy);
  if (n < 0)
    abort();
  va_end(args_copy);
  if (n < leftover)
    h->length += n;
  else {
    _buffer_grow(h, h->length + n);
    r = h->data + h->length;
    va_copy(args_copy, args);
    int n2 = vsnprintf(r, n + 1, fmt, args_copy);
    if (n != n2)
      abort(); // should never happen!
    va_end(args_copy);
    h->length += n;
  }
}

void buffer_appendf(buffer_t *h, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  buffer_appendvf(h, fmt, args);
  va_end(args);
}
```


Most objects and code manipulate data.  Buffer isn't an exception. I often design software around data.  The `buffer_t` structure is the input to all of the functions except `buffer_init`, which creates it.  In C, it is common to name a structure `name_s` and then `typedef` that to be `name_t`.  `name_s and `name_t` are the same things.  To refer to `name_s`, you must include the keyword struct before it.  The `name_t` doesn't require the `struct` keyword to precede the name (it doesn't work).  

The code in buffer.h indicates that the buffer_t is a new type (struct buffer_s) and that the members of buffer_s are hidden.  By hiding the members of buffer_s in the implementation, one can force users to only work through the interface.
```c
struct buffer_s;
typedef struct buffer_s buffer_t;
```

The buffer_s structure is defined as follows in buffer.c.  buffer_s maintains both a length and a size (and data). `data` is always size bytes in length (+1).  `length` is the number of bytes that are currently in use by the buffer within `data`.
```c
struct buffer_s {
  char *data;
  size_t length;
  size_t size;
};
```

The `buffer_init` function initializes the object as being empty (length=0) and sets the data to the `initial_size` (+1).  In C, strings terminate with a zero byte.  I've found it handy in the buffer object to keep a zero character at the end of the buffer.
```c
buffer_t *buffer_init(size_t initial_size) {
  buffer_t *h = (buffer_t *)malloc(sizeof(buffer_t));
  /* notice the +1 to allow for zero terminator */
  h->data = (char *)malloc(initial_size + 1);
  h->data[0] = 0;
  h->length = 0;
  h->size = initial_size;
  return h;
}
```

Objects typically allocate as:

```c
object_name_t *h = (object_name *)malloc(sizeof(object_name_t));
```

This allocates the space needed (the size of the `buffer_t` structure) from the heap:
```c
buffer_t *h = (buffer_t *)malloc(sizeof(buffer_t));
```

The data member needs to be initially allocated to be `initial_size` + 1 bytes:
```c
h->data = (char *)malloc(initial_size + 1);
```

Because the initial value of length is zero, set the first byte to zero:
```c
h->data[0] = 0;
```

Set the initial length to zero:
```c
h->length = 0;
```

Set the size to the `initial_size` passed into the `buffer_init` function:
```c
h->size = initial_size;
```

Return the initialized `buffer_t` object:
```c
return h;
```

You may notice that I don't check the return from malloc.  If malloc fails, it returns `NULL`.  The line after both malloc calls cause the program to abort, which is the only thing I'd want to do anyway.  For this reason, there isn't a point in having a line like `if(!h) abort();`.


The `buffer_destroy` method frees up the resources used by `buffer_init`.
```c
void buffer_destroy(buffer_t *h) {
  free(h->data);
  free(h);
}
```

In C, you must keep track of the pointer to memory that you allocate through malloc so that it can be freed later using free.  We will develop objects which will keep track of allocation for us.  These objects follow a typical pattern which reduces the chance that end-users make a mistake in this regard.

The buffer maintains a single contiguous block of memory that may change as you add more data to it (if the current block of memory is too small, for example).  Clearing the buffer doesn't free memory.  It only resets the length counter to 0 and sets the zero terminator to the 0th byte.
```c
void buffer_clear(buffer_t *h) {
  h->length = 0;
  h->data[0] = 0;
}
```

To access the data, call `buffer_data`.  The implementation simply returns the data member from the structure.
```c
char *buffer_data(buffer_t *h) { return h->data; }
```

Similarly, you can get the length of your data by calling `buffer_length`.
```c
size_t buffer_length(buffer_t *h) { return h->length; }
```

To set bytes into the buffer, you can use `buffer_set` which implements as follows.  The buffer doesn't make a distinction between binary data and text.  You can set (or append) binary data or text through this function.
```c
void buffer_set(buffer_t *h, const void *data, size_t length) {
  _buffer_set(h, data, length);
}
```

The function above calls an internal inlined function to set bytes into the buffer.
```c
static inline void _buffer_set(buffer_t *h, const void *data, size_t length) {
  if (length > h->size)
    _buffer_alloc(h, length);
  memcpy(h->data, data, length);
  h->length = length;
  h->data[length] = 0;
}
```

If length is greater than the size member (h->size), then the buffer needs to be resized using `_buffer_alloc`
```c
if (length > h->size)
  _buffer_alloc(h, length);
```

The inline `_buffer_alloc` method.
```c
static inline void _buffer_alloc(buffer_t *h, size_t length) {
  size_t len = (length + 50) + (h->size >> 3);
  free(h->data);
  h->data = (char *)malloc(len + 1);
  h->size = len;
}
```

Our new length will be the requested length + 50 + (the old size / 8).  `size_t` is a type that is defined in `stdio.h`.
```c
size_t len = (length + 50) + (h->size >> 3);
```

Because we are setting the value, we don't need to retain the old data.  We can free it:
```c
free(h->data);
```

Allocate data to the new size and set the size parameter accordingly:
```c
h->data = (char *)malloc(len + 1);
h->size = len;
```

In `_buffer_set` again, copy data which is length bytes long into h->data.
```c
memcpy(h->data, data, length);
```

Set the length of the buffer to be length and set the zero terminator.
```c
h->length = length;
h->data[length] = 0;
```

Similar to `buffer_set`, `buffer_append` just calls `_buffer_append`.
```c
void buffer_append(buffer_t *h, const void *data, size_t length) {
  _buffer_append(h, data, length);
}
```

The key differences are that the new size will be h->length + length instead of just length.  `_buffer_grow` makes a copy of the old data if the memory has to be reallocated.
```c
static inline void _buffer_append(buffer_t *h, const void *data,
                                  size_t length) {
  if (h->length + length > h->size)
    _buffer_grow(h, h->length + length);

  memcpy(h->data + h->length, data, length);
  h->length += length;
  h->data[h->length] = 0;
}
```

The new length is the same as `_buffer_alloc` (length+50+(h->size/8)).  The new memory malloces into data, and the current h->data is copied to the new memory before freeing h->data.
```c
static inline void _buffer_grow(buffer_t *h, size_t length) {
  size_t len = (length + 50) + (h->size >> 3);
  char *data = (char *)malloc(len + 1);
  memcpy(data, h->data, h->length + 1);
  free(h->data);
  h->data = data;
  h->size = len;
}
```

`buffer_sets` uses `_buffer_set` for its implementation using `strlen` to determine length:
```c
void buffer_sets(buffer_t *h, const char *s) { _buffer_set(h, s, strlen(s)); }
```

`buffer_appends` appends a string.  `buffer_appends` calls `_buffer_append` with the `strlen` of s:
```c
void buffer_appends(buffer_t *h, const char *s) {
  _buffer_append(h, s, strlen(s));
}
```

`buffer_setc` uses `_buffer_set` and takes the address of c and passes a length of 1.
```c
void buffer_setc(buffer_t *h, char c) { _buffer_set(h, &c, 1); }
```

This method is optimized a bit since we are only adding exactly one byte.  Appending occurs more frequently than setting, so it is optimized.
```c
void buffer_appendc(buffer_t *h, char ch) {
  if (h->length + 1 > h->size)
    _buffer_grow(h, h->length + 1);

  char *d = h->data + h->length;
  *d++ = ch;
  *d = 0;
  h->length++;
}
```

`buffer_setn` sets the length to zero (clearing buffer) and then calls `buffer_appendn`:
```c
void buffer_setn(buffer_t *h, char ch, ssize_t n) {
  h->length = 0;
  buffer_appendn(h, ch, n);
}
```

`buffer_appendn` appends `ch` n times to the buffer.  Using `memset` instead of `memcpy` to repeat character n times:
```
void buffer_appendn(buffer_t *h, char ch, ssize_t n) {
  if (n <= 0)
    return;

  if (h->length + n > h->size)
    _buffer_grow(h, h->length + n);

  char *d = h->data + h->length;
  memset(d, ch, n);
  d += n;
  *d = 0;
  h->length += n;
}
```


`buffer_setvf` sets the length to zero (clearing buffer) and then calls `buffer_appendvf`.  `va_list` is defined in `stdarg.h`
```
void buffer_setvf(buffer_t *h, const char *fmt, va_list args) {
  h->length = 0;
  buffer_appendvf(h, fmt, args);
}
```

`buffer_setf` sets the length to zero (clearing buffer).  It then associates a `va_list` variable to the parameter `fmt` (which is considered a format string).  The format string is ultimately used much like `printf` (search for "c printf tutorial" to find more about it).  `buffer_appendvf` is called and then the args variable's resources are freed.  You can make custom functions that set or append to a buffer in much the same way as the `setf/appendf` methods.  Of course, you will use `buffer_clear` before `buffer_appendvf` if you wish to implement a custom set method.
```c
void buffer_setf(buffer_t *h, const char *fmt, ...) {
  h->length = 0;
  va_list args;
  va_start(args, fmt);
  buffer_appendvf(h, fmt, args);
  va_end(args);
}
```

`buffer_appendf` is identical to `buffer_setf`, except that the length isn't reset (h->length = 0 doesn't exist).
```c
void buffer_appendf(buffer_t *h, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  buffer_appendvf(h, fmt, args);
  va_end(args);
}
```

This method is used to append a formatted string to a buffer and probably one of the more complex functions in buffer.  `vsnprintf` returns the number of bytes needed to print a given string.  Because the buffer has reserve memory, the leftover (or reserved) memory is frequently enough, and the buffer doesn't need to grow.  I'll break it down afterward.
```c
void buffer_appendvf(buffer_t *h, const char *fmt, va_list args) {
  va_list args_copy;
  va_copy(args_copy, args);
  size_t leftover = h->size - h->length;
  char *r = h->data + h->length;
  int n = vsnprintf(r, leftover, fmt, args_copy);
  if (n < 0)
    abort();
  va_end(args_copy);
  if (n < leftover) {
    r[n] = 0;
    h->length += n;
  } else {
    _buffer_grow(h, h->length + n);
    r = h->data + h->length;
    va_copy(args_copy, args);
    int n2 = vsnprintf(r, n + 1, fmt, args_copy);
    if (n != n2)
      abort(); // should never happen!
    va_end(args_copy);
    h->length += n;
  }
}
```

Before calling `vsnprintf`, a copy of the args should be made as follows.  The `va_end` should be called once done with the copy of the args.
```c
va_list args_copy;
va_copy(args_copy, args);
... // call vsnprintf
va_end(args_copy);
```

Get the number of bytes leftover in the buffer.  Maybe `vsnprintf` will only have to be called once!
```c
size_t leftover = h->size - h->length;
```

Get a pointer to the end of the current data (to append the string)
```c
char *r = h->data + h->length;
```

`vsnprintf` requires a buffer (r), and a length of the buffer (leftover), fmt, and va_list (args_copy).  It returns the number of bytes needed to be successful.  If n is -1, something bad went wrong.  Just abort (maybe this isn't the best decision?)
```c
int n = vsnprintf(r, leftover, fmt, args_copy);
if (n < 0)
  abort();
```

If n is less than leftover, then the write was successful, and we are done.  Add n bytes to the overall length.
```c
if (n < leftover)
  h->length += n;
```

If n is greater or equal to leftover, the buffer size must increase to the current length + n bytes.
```c
else {
  _buffer_grow(h, h->length + n);
```

Get a pointer to the end of the current data (to append the string)
```c
  r = h->data + h->length;
```

Copy the args like before.
```c
  va_copy(args_copy, args);
```

The memory needed for this to complete is 1 byte more than n because `vsnprintf` returns the number of bytes - 1 for the zero terminator.  Once finished, add n bytes to length.
```c
  int n2 = vsnprintf(r, n + 1, fmt, args_copy);
  if (n != n2)
    abort(); // should never happen!
  va_end(args_copy);
  h->length += n;
```

`buffer_alloc` resizes the buffer to length bytes and returns you a pointer to the data.  The old data may be overridden.
```c
void *buffer_alloc(buffer_t *h, size_t length) {
  if (length > h->size)
    _buffer_alloc(h, length);
  h->length = length;
  h->data[h->length] = 0;
  return h->data;
}
```

`buffer_resize` allows you to alter the size of the buffer and then returns a pointer to the data.  The resize does not override old data (except if resizing to a smaller length).  If resizing to a smaller length, a zero writes the length byte.
```c
void *buffer_resize(buffer_t *h, size_t length) {
  if (length > h->size)
    _buffer_grow(h, length);
  h->length = length;
  h->data[h->length] = 0;
  return h->data;
}
```

`buffer_append_alloc` is like `buffer_resize`, except that it allocates length more bytes and returns a pointer to those new bytes.  It is safe to assume that the byte after length is set to zero.
```c
void *buffer_append_alloc(buffer_t *h, size_t length) {
  if (length + h->length > h->size)
    _buffer_grow(h, length + h->length);
  char *r = h->data + h->length;
  h->length += length;
  r[h->length] = 0;
  return r;
}
```

If you followed everything in this, that is great.  Otherwise, it might make more sense as the buffer object is used throughout the project.  Next up is 2_linked_structures.md

# [Linked Lists](4_linked_lists.md)

[Table of Contents](README.md)  - Copyright 2019 Andy Curtis
