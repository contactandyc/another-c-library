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

#include <stdlib.h>

struct ac_buffer_s {
#ifdef _AC_MEMORY_CHECK_
  ac_allocator_dump_t dump;
  size_t initial_size;
  size_t max_length;
#endif
  char *data;
  size_t length;
  size_t size;
  ac_pool_t *pool;
};

static inline ac_buffer_t *ac_buffer_pool_init(ac_pool_t *pool,
                                               size_t initial_size) {
  ac_buffer_t *h = (ac_buffer_t *)ac_pool_calloc(pool, sizeof(ac_buffer_t));
  h->data = (char *)ac_pool_alloc(pool, initial_size + 1);
  h->data[0] = 0;
  h->size = initial_size;
  h->pool = pool;
  return h;
}

static inline void ac_buffer_clear(ac_buffer_t *h) {
  h->length = 0;
  h->data[0] = 0;
}

static inline char *ac_buffer_data(ac_buffer_t *h) { return h->data; }
static inline size_t ac_buffer_length(ac_buffer_t *h) { return h->length; }
static inline char *ac_buffer_end(ac_buffer_t *h) {
  return h->data + h->length;
}

static inline void _ac_buffer_grow(ac_buffer_t *h, size_t length) {
  size_t len = (length + 50) + (h->size >> 3);
  if (!h->pool) {
    char *data = (char *)ac_malloc(len + 1);
    memcpy(data, h->data, h->length + 1);
    if (h->size)
      ac_free(h->data);
    h->data = data;
  } else
    h->data = (char *)ac_pool_dup(h->pool, h->data, len + 1);
  h->size = len;
}

static inline void *ac_buffer_shrink_by(ac_buffer_t *h, size_t length) {
  if (h->length > length)
    h->length -= length;
  else
    h->length = 0;
  h->data[h->length] = 0;
  return h->data;
}

static inline void *ac_buffer_resize(ac_buffer_t *h, size_t length) {
  if (length > h->size)
    _ac_buffer_grow(h, length);
  h->length = length;
  h->data[h->length] = 0;
#ifdef _AC_MEMORY_CHECK_
  if (length > h->max_length)
    h->max_length = length;
#endif
  return h->data;
}

static inline void *ac_buffer_append_alloc(ac_buffer_t *h, size_t length) {
  size_t m = h->length & 7;
  if (m > 0) {
    m = 8 - m;
    if (m + h->length > h->size)
      _ac_buffer_grow(h, m + h->length);
    h->length += m;
    h->data[h->length] = 0;
  }

  if (length + h->length > h->size)
    _ac_buffer_grow(h, length + h->length);
  char *r = h->data + h->length;
  h->length += length;
  r[length] = 0;
#ifdef _AC_MEMORY_CHECK_
  if (h->length > h->max_length)
    h->max_length = h->length;
#endif
  return r;
}

static inline void *ac_buffer_append_ualloc(ac_buffer_t *h, size_t length) {
  if (length + h->length > h->size)
    _ac_buffer_grow(h, length + h->length);
  char *r = h->data + h->length;
  h->length += length;
  r[length] = 0;
#ifdef _AC_MEMORY_CHECK_
  if (h->length > h->max_length)
    h->max_length = h->length;
#endif
  return r;
}

void _ac_buffer_append(ac_buffer_t *h, const void *data, size_t length);

static inline void ac_buffer_append(ac_buffer_t *h, const void *data,
                                    size_t length) {
  _ac_buffer_append(h, data, length);
}

static inline void ac_buffer_appends(ac_buffer_t *h, const char *s) {
  _ac_buffer_append(h, s, strlen(s));
}

static inline void ac_buffer_appendc(ac_buffer_t *h, char ch) {
  if (h->length + 1 > h->size)
    _ac_buffer_grow(h, h->length + 1);

  char *d = h->data + h->length;
  *d++ = ch;
  *d = 0;
  h->length++;
#ifdef _AC_MEMORY_CHECK_
  if (h->length > h->max_length)
    h->max_length = h->length;
#endif
}

static inline void ac_buffer_appendn(ac_buffer_t *h, char ch, ssize_t n) {
  if (n <= 0)
    return;

  if (h->length + n > h->size)
    _ac_buffer_grow(h, h->length + n);

  char *d = h->data + h->length;
  memset(d, ch, n);
  d += n;
  *d = 0;
  h->length += n;
#ifdef _AC_MEMORY_CHECK_
  if (h->length > h->max_length)
    h->max_length = h->length;
#endif
}

static inline void _ac_buffer_alloc(ac_buffer_t *h, size_t length) {
  size_t len = (length + 50) + (h->size >> 3);
  if (!h->pool) {
    if (h->size)
      ac_free(h->data);
    h->data = (char *)ac_malloc(len + 1);
  } else
    h->data = (char *)ac_pool_alloc(h->pool, len + 1);
  h->size = len;
}

static inline void *ac_buffer_alloc(ac_buffer_t *h, size_t length) {
  if (length > h->size)
    _ac_buffer_alloc(h, length);
  h->length = length;
#ifdef _AC_MEMORY_CHECK_
  if (length > h->max_length)
    h->max_length = length;
#endif
  h->data[h->length] = 0;
  return h->data;
}

static inline void _ac_buffer_set(ac_buffer_t *h, const void *data,
                                  size_t length) {
  if (length > h->size)
    _ac_buffer_alloc(h, length);
  memcpy(h->data, data, length);
  h->length = length;
#ifdef _AC_MEMORY_CHECK_
  if (length > h->max_length)
    h->max_length = length;
#endif
  h->data[length] = 0;
}

static inline void ac_buffer_set(ac_buffer_t *h, const void *data,
                                 size_t length) {
  _ac_buffer_set(h, data, length);
}

static inline void ac_buffer_sets(ac_buffer_t *h, const char *s) {
  _ac_buffer_set(h, s, strlen(s));
}

static inline void ac_buffer_setc(ac_buffer_t *h, char c) {
  _ac_buffer_set(h, &c, 1);
}

static inline void ac_buffer_setn(ac_buffer_t *h, char ch, ssize_t n) {
  h->length = 0;
  ac_buffer_appendn(h, ch, n);
}

static inline void ac_buffer_setvf(ac_buffer_t *h, const char *fmt,
                                   va_list args) {
  h->length = 0;
  ac_buffer_appendvf(h, fmt, args);
}

static inline void ac_buffer_setf(ac_buffer_t *h, const char *fmt, ...) {
  h->length = 0;
  va_list args;
  va_start(args, fmt);
  ac_buffer_appendvf(h, fmt, args);
  va_end(args);
}

static inline void ac_buffer_appendf(ac_buffer_t *h, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ac_buffer_appendvf(h, fmt, args);
  va_end(args);
}
