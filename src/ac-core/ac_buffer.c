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

#include "another-c-library/ac_buffer.h"

#include <stdlib.h>
#include <string.h>

#ifdef _AC_MEMORY_CHECK_
static void dump_buffer(FILE *out, const char *caller, void *p, size_t length) {
  ac_buffer_t *bh = (ac_buffer_t *)p;
  fprintf(out, "%s size: %lu, max_length: %lu, initial_size: %lu ", caller,
          bh->size, bh->max_length, bh->initial_size);
}

ac_buffer_t *_ac_buffer_init(size_t initial_size, const char *caller) {
  ac_buffer_t *h = (ac_buffer_t *)_ac_malloc_d(
      NULL, caller, sizeof(ac_buffer_t), true);
  h->dump.dump = dump_buffer;
  h->initial_size = initial_size;
  h->max_length = 0;
#else
ac_buffer_t *_ac_buffer_init(size_t initial_size) {
  ac_buffer_t *h = (ac_buffer_t *)ac_malloc(sizeof(ac_buffer_t));
#endif
  h->data = initial_size ? (char *)ac_malloc(initial_size + 1)
                         : (char *)(&(h->size));
  h->data[0] = 0;
  h->length = 0;
  h->size = initial_size;
  h->pool = NULL;
  return h;
}

void ac_buffer_destroy(ac_buffer_t *h) {
  if (!h->pool) {
    ac_free(h->data);
    ac_free(h);
  }
}

void _ac_buffer_append(ac_buffer_t *h, const void *data, size_t length) {
  if (h->length + length > h->size)
    _ac_buffer_grow(h, h->length + length);

  memcpy(h->data + h->length, data, length);
  h->length += length;
  h->data[h->length] = 0;
#ifdef _AC_MEMORY_CHECK_
  if (length > h->max_length)
    h->max_length = length;
#endif
}

void ac_buffer_appendvf(ac_buffer_t *h, const char *fmt, va_list args) {
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
    _ac_buffer_grow(h, h->length + n);
    r = h->data + h->length;
    va_copy(args_copy, args);
    int n2 = vsnprintf(r, n + 1, fmt, args_copy);
    if (n != n2)
      abort(); // should never happen!
    va_end(args_copy);
    h->length += n;
  }
#ifdef _AC_MEMORY_CHECK_
  if (h->length > h->max_length)
    h->max_length = h->length;
#endif
}
