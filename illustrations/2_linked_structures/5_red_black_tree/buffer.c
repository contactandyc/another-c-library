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
