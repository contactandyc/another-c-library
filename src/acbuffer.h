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

#ifndef _acbuffer_H
#define _acbuffer_H

#include "acallocator.h"
#include "acpool.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

struct acbuffer_s;
typedef struct acbuffer_s acbuffer_t;

/* set the buffer to an initial size, buffer will grow as needed */
#ifdef _ACDEBUG_MEMORY_
#define acbuffer_init(size)                                                 \
  _acbuffer_init(size, ACFILE_LINE_MACRO("acbuffer"));
acbuffer_t *_acbuffer_init(size_t size, const char *caller);
#else
#define acbuffer_init(size) _acbuffer_init(size)
acbuffer_t *_acbuffer_init(size_t size);
#endif

/* like above, except allocated with a pool (no need to destroy) */
static inline acbuffer_t *acbuffer_pool_init(acpool_t *pool,
                                                   size_t initial_size);

/* clear the buffer */
static inline void acbuffer_clear(acbuffer_t *h);

/* get the contents of the buffer */
static inline char *acbuffer_data(acbuffer_t *h);
/* get the length of the buffer */
static inline size_t acbuffer_length(acbuffer_t *h);

/* Functions to append contents into a buffer (vs set). */
/* append bytes to the current buffer */
static inline void acbuffer_append(acbuffer_t *h, const void *data,
                                      size_t length);

/* append a string to the current buffer */
static inline void acbuffer_appends(acbuffer_t *h, const char *s);

/* append a character to the current buffer */
static inline void acbuffer_appendc(acbuffer_t *h, char ch);

/* append a character n times to the current buffer */
static inline void acbuffer_appendn(acbuffer_t *h, char ch, ssize_t n);

/* append bytes in current buffer using va_args and formatted string */
void acbuffer_appendvf(acbuffer_t *h, const char *fmt, va_list args);

/* append bytes in current buffer using a formatted string -similar to printf */
static inline void acbuffer_appendf(acbuffer_t *h, const char *fmt, ...);

/* Functions to set the contents into a buffer (vs append). */
/* set bytes to the current buffer */
static inline void acbuffer_set(acbuffer_t *h, const void *data,
                                   size_t length);

/* set a string to the current buffer */
static inline void acbuffer_sets(acbuffer_t *h, const char *s);

/* set a character to the current buffer */
static inline void acbuffer_setc(acbuffer_t *h, char ch);

/* set a character n times to the current buffer */
static inline void acbuffer_setn(acbuffer_t *h, char ch, ssize_t n);

/* set bytes in current buffer using va_args and formatted string */
static inline void acbuffer_setvf(acbuffer_t *h, const char *fmt,
                                     va_list args);

/* set bytes in current buffer using a formatted string -similar to printf */
static inline void acbuffer_setf(acbuffer_t *h, const char *fmt, ...);

/* resize the buffer and return a pointer to the beginning of the buffer.  This
   will retain the original data in the buffer for up to length bytes. */
static inline void *acbuffer_resize(acbuffer_t *h, size_t length);

/* shrink the buffer by length bytes, if the buffer is not length bytes, buffer
   will be cleared. */
static inline void *acbuffer_shrink_by(acbuffer_t *h, size_t length);

/* grow the buffer by length bytes and return pointer to the new memory.  This
   will retain the original data in the buffer for up to length bytes. */
static inline void *acbuffer_append_alloc(acbuffer_t *h, size_t length);

/* resize the buffer and return a pointer to the beginning of the buffer.  This
   will NOT retain the original data in the buffer for up to length bytes. */
static inline void *acbuffer_alloc(acbuffer_t *h, size_t length);

/* destroy the buffer */
void acbuffer_destroy(acbuffer_t *h);

#include "impl/acbuffer.h"

#endif
