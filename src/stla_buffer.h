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

#ifndef _stla_buffer_H
#define _stla_buffer_H

#include "stla_allocator.h"
#include "stla_pool.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

struct stla_buffer_s;
typedef struct stla_buffer_s stla_buffer_t;

/* set the buffer to an initial size, buffer will grow as needed */
#ifdef _STLA_DEBUG_MEMORY_
#define stla_buffer_init(size)                                                 \
  _stla_buffer_init(size, STLA_FILE_LINE_MACRO("stla_buffer"));
stla_buffer_t *_stla_buffer_init(size_t size, const char *caller);
#else
#define stla_buffer_init(size) _stla_buffer_init(size)
stla_buffer_t *_stla_buffer_init(size_t size);
#endif

/* like above, except allocated with a pool (no need to destroy) */
static inline stla_buffer_t *stla_buffer_pool_init(stla_pool_t *pool,
                                                   size_t initial_size);

/* clear the buffer */
static inline void stla_buffer_clear(stla_buffer_t *h);

/* get the contents of the buffer */
static inline char *stla_buffer_data(stla_buffer_t *h);
/* get the length of the buffer */
static inline size_t stla_buffer_length(stla_buffer_t *h);

/* Functions to append contents into a buffer (vs set). */
/* append bytes to the current buffer */
static inline void stla_buffer_append(stla_buffer_t *h, const void *data,
                                      size_t length);

/* append a string to the current buffer */
static inline void stla_buffer_appends(stla_buffer_t *h, const char *s);

/* append a character to the current buffer */
static inline void stla_buffer_appendc(stla_buffer_t *h, char ch);

/* append a character n times to the current buffer */
static inline void stla_buffer_appendn(stla_buffer_t *h, char ch, ssize_t n);

/* append bytes in current buffer using va_args and formatted string */
void stla_buffer_appendvf(stla_buffer_t *h, const char *fmt, va_list args);

/* append bytes in current buffer using a formatted string -similar to printf */
static inline void stla_buffer_appendf(stla_buffer_t *h, const char *fmt, ...);

/* Functions to set the contents into a buffer (vs append). */
/* set bytes to the current buffer */
static inline void stla_buffer_set(stla_buffer_t *h, const void *data,
                                   size_t length);

/* set a string to the current buffer */
static inline void stla_buffer_sets(stla_buffer_t *h, const char *s);

/* set a character to the current buffer */
static inline void stla_buffer_setc(stla_buffer_t *h, char ch);

/* set a character n times to the current buffer */
static inline void stla_buffer_setn(stla_buffer_t *h, char ch, ssize_t n);

/* set bytes in current buffer using va_args and formatted string */
static inline void stla_buffer_setvf(stla_buffer_t *h, const char *fmt,
                                     va_list args);

/* set bytes in current buffer using a formatted string -similar to printf */
static inline void stla_buffer_setf(stla_buffer_t *h, const char *fmt, ...);

/* resize the buffer and return a pointer to the beginning of the buffer.  This
   will retain the original data in the buffer for up to length bytes. */
static inline void *stla_buffer_resize(stla_buffer_t *h, size_t length);

/* grow the buffer by length bytes and return pointer to the new memory.  This
   will retain the original data in the buffer for up to length bytes. */
static inline void *stla_buffer_append_alloc(stla_buffer_t *h, size_t length);

/* resize the buffer and return a pointer to the beginning of the buffer.  This
   will NOT retain the original data in the buffer for up to length bytes. */
static inline void *stla_buffer_alloc(stla_buffer_t *h, size_t length);

/* destroy the buffer */
void stla_buffer_destroy(stla_buffer_t *h);

#include "impl/stla_buffer.h"

#endif
