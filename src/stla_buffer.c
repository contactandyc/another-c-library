#include "stla_buffer.h"

#include <stdlib.h>
#include <string.h>

stla_buffer_t *stla_buffer_init(size_t initial_size) {
  stla_buffer_t *h = (stla_buffer_t *)malloc(sizeof(stla_buffer_t));
  h->data = (char *)malloc(initial_size + 1);
  h->data[0] = 0;
  h->length = 0;
  h->size = initial_size;
  h->pool = NULL;
  return h;
}

void stla_buffer_destroy(stla_buffer_t *h) {
  if(!h->pool) {
    free(h->data);
    free(h);
  }
}

void _stla_buffer_append(stla_buffer_t *h, const void *data, size_t length) {
  if (h->length + length > h->size)
    _stla_buffer_grow(h, h->length + length);

  memcpy(h->data + h->length, data, length);
  h->length += length;
  h->data[h->length] = 0;
}

void stla_buffer_appendvf(stla_buffer_t *h, const char *fmt, va_list args) {
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
    _stla_buffer_grow(h, h->length + n);
    r = h->data + h->length;
    va_copy(args_copy, args);
    int n2 = vsnprintf(r, n + 1, fmt, args_copy);
    if (n != n2)
      abort(); // should never happen!
    va_end(args_copy);
    h->length += n;
  }
}
