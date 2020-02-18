/*
Copyright 2019 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _ac_buffer_H
#define _ac_buffer_H

/*
  The ac_buffer object is much like the C++ string class in that it supports
  a number of easy to use string methods and a buffer acts like a string in
  that a buffer grows as needed to fit the contents of the string.  The object
  is more than a string object in as much as it also equally allows for
  binary data to be appended to or a combination of binary data and strings.
  At it's core, it is simply a buffer that will auto-resize as needed.  The
  pool object is different in that it is used for lots of smaller allocations.
  The buffer generally is used to track "one thing".  That one thing might
  consist of many parts, but they are generally all tracked together in a
  contiguous space.
*/

#include "ac_allocator.h"
#include "ac_pool.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ac_buffer_s;
typedef struct ac_buffer_s ac_buffer_t;

/* ac_buffer_init creates a buffer with an initial size of size.  The buffer
   will grow as needed, but if you know the size that is generally needed,
   it may be more efficient to initialize it to that size.

   ac_buffer_t *ac_buffer_init(size_t size);
*/
#ifdef _AC_DEBUG_MEMORY_
#define ac_buffer_init(size)                                                   \
  _ac_buffer_init(size, AC_FILE_LINE_MACRO("ac_buffer"));
ac_buffer_t *_ac_buffer_init(size_t size, const char *caller);
#else
#define ac_buffer_init(size) _ac_buffer_init(size)
ac_buffer_t *_ac_buffer_init(size_t size);
#endif

/* like above, except allocated with a pool (no need to destroy) */
static inline ac_buffer_t *ac_buffer_pool_init(ac_pool_t *pool,
                                               size_t initial_size);

/* clear the buffer */
static inline void ac_buffer_clear(ac_buffer_t *h);

/* get the contents of the buffer */
static inline char *ac_buffer_data(ac_buffer_t *h);

/* get the contents of the buffer at pos (or the end if passed the end) */
static inline char *ac_buffer_at(ac_buffer_t *h, size_t pos);

/* get the length of the buffer */
static inline size_t ac_buffer_length(ac_buffer_t *h);

/* get the end of contents of the buffer */
static inline char *ac_buffer_end(ac_buffer_t *h);

/* Functions to append contents into a buffer (vs set). */
/* append bytes to the current buffer */
static inline void ac_buffer_append(ac_buffer_t *h, const void *data,
                                    size_t length);

/* append a string to the current buffer */
static inline void ac_buffer_appends(ac_buffer_t *h, const char *s);

/* append a character to the current buffer */
static inline void ac_buffer_appendc(ac_buffer_t *h, char ch);

/* append a character n times to the current buffer */
static inline void ac_buffer_appendn(ac_buffer_t *h, char ch, ssize_t n);

/* append bytes in current buffer using va_args and formatted string */
void ac_buffer_appendvf(ac_buffer_t *h, const char *fmt, va_list args);

/* append bytes in current buffer using a formatted string -similar to printf */
static inline void ac_buffer_appendf(ac_buffer_t *h, const char *fmt, ...);

/* Functions to set the contents into a buffer (vs append). */
/* set bytes to the current buffer */
static inline void ac_buffer_set(ac_buffer_t *h, const void *data,
                                 size_t length);

/* set a string to the current buffer */
static inline void ac_buffer_sets(ac_buffer_t *h, const char *s);

/* set a character to the current buffer */
static inline void ac_buffer_setc(ac_buffer_t *h, char ch);

/* set a character n times to the current buffer */
static inline void ac_buffer_setn(ac_buffer_t *h, char ch, ssize_t n);

/* set bytes in current buffer using va_args and formatted string */
static inline void ac_buffer_setvf(ac_buffer_t *h, const char *fmt,
                                   va_list args);

/* set bytes in current buffer using a formatted string -similar to printf */
static inline void ac_buffer_setf(ac_buffer_t *h, const char *fmt, ...);

/* resize the buffer and return a pointer to the beginning of the buffer.  This
   will retain the original data in the buffer for up to length bytes. */
static inline void *ac_buffer_resize(ac_buffer_t *h, size_t length);

/* shrink the buffer by length bytes, if the buffer is not length bytes, buffer
   will be cleared. */
static inline void *ac_buffer_shrink_by(ac_buffer_t *h, size_t length);

/* grow the buffer by length bytes and return pointer to the new memory.  This
   will retain the original data in the buffer for up to length bytes. */
static inline void *ac_buffer_append_alloc(ac_buffer_t *h, size_t length);

/* same as append_alloc except memory is not necessarily aligned */
static inline void *ac_buffer_append_ualloc(ac_buffer_t *h, size_t length);

/* resize the buffer and return a pointer to the beginning of the buffer.  This
   will NOT retain the original data in the buffer for up to length bytes. */
static inline void *ac_buffer_alloc(ac_buffer_t *h, size_t length);

/* destroy the buffer */
void ac_buffer_destroy(ac_buffer_t *h);

#include "impl/ac_buffer.h"

#ifdef __cplusplus
}
#endif

#endif
