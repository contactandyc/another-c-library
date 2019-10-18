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

struct acbuffer_s {
#ifdef _ACDEBUG_MEMORY_
  acallocator_dump_t dump;
  size_t initial_size;
  size_t max_length;
#endif
  char *data;
  size_t length;
  size_t size;
  acpool_t *pool;
};

static inline acbuffer_t *acbuffer_pool_init(acpool_t *pool,
                                                   size_t initial_size) {
  acbuffer_t *h =
      (acbuffer_t *)acpool_alloc(pool, sizeof(acbuffer_t));
  h->data = (char *)acpool_alloc(pool, initial_size + 1);
  h->data[0] = 0;
  h->length = 0;
  h->size = initial_size;
  h->pool = pool;
  return h;
}

static inline void acbuffer_clear(acbuffer_t *h) {
  h->length = 0;
  h->data[0] = 0;
}

static inline char *acbuffer_data(acbuffer_t *h) { return h->data; }
static inline size_t acbuffer_length(acbuffer_t *h) { return h->length; }

static inline void _acbuffer_grow(acbuffer_t *h, size_t length) {
  size_t len = (length + 50) + (h->size >> 3);
  if (!h->pool) {
    char *data = (char *)acmalloc(len + 1);
    memcpy(data, h->data, h->length + 1);
    if (h->size)
      acfree(h->data);
    h->data = data;
  } else
    h->data = (char *)acpool_dup(h->pool, h->data, len + 1);
  h->size = len;
}

static inline void *acbuffer_shrink_by(acbuffer_t *h, size_t length) {
  if (h->length > length)
    h->length -= length;
  else
    h->length = 0;
  h->data[h->length] = 0;
  return h->data;
}

static inline void *acbuffer_resize(acbuffer_t *h, size_t length) {
  if (length > h->size)
    _acbuffer_grow(h, length);
  h->length = length;
  h->data[h->length] = 0;
#ifdef _ACDEBUG_MEMORY_
  if (length > h->max_length)
    h->max_length = length;
#endif
  return h->data;
}

static inline void *acbuffer_append_alloc(acbuffer_t *h, size_t length) {
  if (length + h->length > h->size)
    _acbuffer_grow(h, length + h->length);
  char *r = h->data + h->length;
  h->length += length;
  r[h->length] = 0;
#ifdef _ACDEBUG_MEMORY_
  if (length > h->max_length)
    h->max_length = length;
#endif
  return r;
}

void _acbuffer_append(acbuffer_t *h, const void *data, size_t length);

static inline void acbuffer_append(acbuffer_t *h, const void *data,
                                      size_t length) {
  _acbuffer_append(h, data, length);
}

static inline void acbuffer_appends(acbuffer_t *h, const char *s) {
  _acbuffer_append(h, s, strlen(s));
}

static inline void acbuffer_appendc(acbuffer_t *h, char ch) {
  if (h->length + 1 > h->size)
    _acbuffer_grow(h, h->length + 1);

  char *d = h->data + h->length;
  *d++ = ch;
  *d = 0;
  h->length++;
#ifdef _ACDEBUG_MEMORY_
  if (h->length > h->max_length)
    h->max_length = h->length;
#endif
}

static inline void acbuffer_appendn(acbuffer_t *h, char ch, ssize_t n) {
  if (n <= 0)
    return;

  if (h->length + n > h->size)
    _acbuffer_grow(h, h->length + n);

  char *d = h->data + h->length;
  memset(d, ch, n);
  d += n;
  *d = 0;
  h->length += n;
#ifdef _ACDEBUG_MEMORY_
  if (h->length > h->max_length)
    h->max_length = h->length;
#endif
}

static inline void _acbuffer_alloc(acbuffer_t *h, size_t length) {
  size_t len = (length + 50) + (h->size >> 3);
  if (!h->pool) {
    if (h->size)
      acfree(h->data);
    h->data = (char *)acmalloc(len + 1);
  } else
    h->data = (char *)acpool_alloc(h->pool, len + 1);
  h->size = len;
}

static inline void *acbuffer_alloc(acbuffer_t *h, size_t length) {
  if (length > h->size)
    _acbuffer_alloc(h, length);
  h->length = length;
#ifdef _ACDEBUG_MEMORY_
  if (length > h->max_length)
    h->max_length = length;
#endif
  h->data[h->length] = 0;
  return h->data;
}

static inline void _acbuffer_set(acbuffer_t *h, const void *data,
                                    size_t length) {
  if (length > h->size)
    _acbuffer_alloc(h, length);
  memcpy(h->data, data, length);
  h->length = length;
#ifdef _ACDEBUG_MEMORY_
  if (length > h->max_length)
    h->max_length = length;
#endif
  h->data[length] = 0;
}

static inline void acbuffer_set(acbuffer_t *h, const void *data,
                                   size_t length) {
  _acbuffer_set(h, data, length);
}

static inline void acbuffer_sets(acbuffer_t *h, const char *s) {
  _acbuffer_set(h, s, strlen(s));
}

static inline void acbuffer_setc(acbuffer_t *h, char c) {
  _acbuffer_set(h, &c, 1);
}

static inline void acbuffer_setn(acbuffer_t *h, char ch, ssize_t n) {
  h->length = 0;
  acbuffer_appendn(h, ch, n);
}

static inline void acbuffer_setvf(acbuffer_t *h, const char *fmt,
                                     va_list args) {
  h->length = 0;
  acbuffer_appendvf(h, fmt, args);
}

static inline void acbuffer_setf(acbuffer_t *h, const char *fmt, ...) {
  h->length = 0;
  va_list args;
  va_start(args, fmt);
  acbuffer_appendvf(h, fmt, args);
  va_end(args);
}

static inline void acbuffer_appendf(acbuffer_t *h, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  acbuffer_appendvf(h, fmt, args);
  va_end(args);
}
