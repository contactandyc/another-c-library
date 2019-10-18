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

#include "acbuffer.h"

#include <stdlib.h>
#include <string.h>

#ifdef _ACDEBUG_MEMORY_
static void dump_buffer(FILE *out, const char *caller, void *p, size_t length) {
  acbuffer_t *bh = (acbuffer_t *)p;
  fprintf(out, "%s size: %lu, max_length: %lu, initial_size: %lu ", caller,
          bh->size, bh->max_length, bh->initial_size);
}

acbuffer_t *_acbuffer_init(size_t initial_size, const char *caller) {
  acbuffer_t *h = (acbuffer_t *)_acmalloc_d(
      NULL, caller, sizeof(acbuffer_t), true);
  h->dump.dump = dump_buffer;
  h->initial_size = initial_size;
  h->max_length = 0;
#else
acbuffer_t *_acbuffer_init(size_t initial_size) {
  acbuffer_t *h = (acbuffer_t *)acmalloc(sizeof(acbuffer_t));
#endif
  h->data = initial_size ? (char *)acmalloc(initial_size + 1)
                         : (char *)(&(h->size));
  h->data[0] = 0;
  h->length = 0;
  h->size = initial_size;
  h->pool = NULL;
  return h;
}

void acbuffer_destroy(acbuffer_t *h) {
  if (!h->pool) {
    acfree(h->data);
    acfree(h);
  }
}

void _acbuffer_append(acbuffer_t *h, const void *data, size_t length) {
  if (h->length + length > h->size)
    _acbuffer_grow(h, h->length + length);

  memcpy(h->data + h->length, data, length);
  h->length += length;
  h->data[h->length] = 0;
#ifdef _ACDEBUG_MEMORY_
  if (length > h->max_length)
    h->max_length = length;
#endif
}

void acbuffer_appendvf(acbuffer_t *h, const char *fmt, va_list args) {
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
    _acbuffer_grow(h, h->length + n);
    r = h->data + h->length;
    va_copy(args_copy, args);
    int n2 = vsnprintf(r, n + 1, fmt, args_copy);
    if (n != n2)
      abort(); // should never happen!
    va_end(args_copy);
    h->length += n;
  }
#ifdef _ACDEBUG_MEMORY_
  if (h->length > h->max_length)
    h->max_length = h->length;
#endif
}
