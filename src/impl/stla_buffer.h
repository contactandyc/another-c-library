#include <stdlib.h>

struct stla_buffer_s {
  char *data;
  size_t length;
  size_t size;
  stla_pool_t *pool;
};


static inline stla_buffer_t *stla_buffer_pool_init(stla_pool_t *pool, size_t initial_size) {
  stla_buffer_t *h = (stla_buffer_t *)stla_pool_alloc(pool, sizeof(stla_buffer_t));
  h->data = (char *)malloc(initial_size + 1);
  h->data[0] = 0;
  h->length = 0;
  h->size = initial_size;
  h->pool = pool;
  return h;
}

static inline void stla_buffer_clear(stla_buffer_t *h) {
  h->length = 0;
  h->data[0] = 0;
}

static inline char *stla_buffer_data(stla_buffer_t *h) { return h->data; }
static inline size_t stla_buffer_length(stla_buffer_t *h) { return h->length; }

static inline void _stla_buffer_grow(stla_buffer_t *h, size_t length) {
  size_t len = (length + 50) + (h->size >> 3);
  if(!h->pool) {
    char *data = (char *)malloc(len + 1);
    memcpy(data, h->data, h->length + 1);
    free(h->data);
    h->data = data;
  }
  else
    h->data = (char *)stla_pool_dup(h->pool, h->data, len+1);
  h->size = len;
}

static inline void *stla_buffer_resize(stla_buffer_t *h, size_t length) {
  if (length > h->size)
    _stla_buffer_grow(h, length);
  h->length = length;
  h->data[h->length] = 0;
  return h->data;
}

static inline void *stla_buffer_append_alloc(stla_buffer_t *h, size_t length) {
  if (length + h->length > h->size)
    _stla_buffer_grow(h, length + h->length);
  char *r = h->data + h->length;
  h->length += length;
  r[h->length] = 0;
  return r;
}

void _stla_buffer_append(stla_buffer_t *h, const void *data, size_t length);

static inline void stla_buffer_append(stla_buffer_t *h, const void *data, size_t length) {
  _stla_buffer_append(h, data, length);
}

static inline void stla_buffer_appends(stla_buffer_t *h, const char *s) {
  _stla_buffer_append(h, s, strlen(s));
}

static inline void stla_buffer_appendc(stla_buffer_t *h, char ch) {
  if (h->length + 1 > h->size)
    _stla_buffer_grow(h, h->length + 1);

  char *d = h->data + h->length;
  *d++ = ch;
  *d = 0;
  h->length++;
}

static inline void stla_buffer_appendn(stla_buffer_t *h, char ch, ssize_t n) {
  if (n <= 0)
    return;

  if (h->length + n > h->size)
    _stla_buffer_grow(h, h->length + n);

  char *d = h->data + h->length;
  memset(d, ch, n);
  d += n;
  *d = 0;
  h->length += n;
}

static inline void _stla_buffer_alloc(stla_buffer_t *h, size_t length) {
  size_t len = (length + 50) + (h->size >> 3);
  if(!h->pool) {
    free(h->data);
    h->data = (char *)malloc(len + 1);
  }
  else
    h->data = (char *)stla_pool_alloc(h->pool, len+1);
  h->size = len;
}

static inline void *stla_buffer_alloc(stla_buffer_t *h, size_t length) {
  if (length > h->size)
    _stla_buffer_alloc(h, length);
  h->length = length;
  h->data[h->length] = 0;
  return h->data;
}

static inline void _stla_buffer_set(stla_buffer_t *h, const void *data, size_t length) {
  if (length > h->size)
    _stla_buffer_alloc(h, length);
  memcpy(h->data, data, length);
  h->length = length;
  h->data[length] = 0;
}

static inline void stla_buffer_set(stla_buffer_t *h, const void *data, size_t length) {
  _stla_buffer_set(h, data, length);
}

static inline void stla_buffer_sets(stla_buffer_t *h, const char *s) { _stla_buffer_set(h, s, strlen(s)); }

static inline void stla_buffer_setc(stla_buffer_t *h, char c) { _stla_buffer_set(h, &c, 1); }

static inline void stla_buffer_setn(stla_buffer_t *h, char ch, ssize_t n) {
  h->length = 0;
  stla_buffer_appendn(h, ch, n);
}

static inline void stla_buffer_setvf(stla_buffer_t *h, const char *fmt, va_list args) {
  h->length = 0;
  stla_buffer_appendvf(h, fmt, args);
}

static inline void stla_buffer_setf(stla_buffer_t *h, const char *fmt, ...) {
  h->length = 0;
  va_list args;
  va_start(args, fmt);
  stla_buffer_appendvf(h, fmt, args);
  va_end(args);
}

static inline void stla_buffer_appendf(stla_buffer_t *h, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  stla_buffer_appendvf(h, fmt, args);
  va_end(args);
}
