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

#include "ac_async_buffer.h"
#include "ac_buffer.h"
#include <stdlib.h>
#include <string.h>

typedef int (*ac_async_buffer_advance_f)(ac_async_buffer_t *);

struct ac_async_buffer_s {
  /*  current delimited chunk  */
  char *chunk_start;
  char *chunk_end;
  /*  full working data set  */
  char *data_start;
  char *data_end;
  /*  callback and user data  */
  void *arg;
  ac_async_buffer_f on_data_ready;
  /*  cache for storing partial data records  */
  ac_buffer_t *buffer;
  int clear_buffer;
  /*  delimiters and advance function for resuming  */
  ac_async_buffer_advance_f delayed_advance;
  size_t bytes;
  char char_delimiter;
  void *mem_delimiter;
  size_t mem_delimiter_length;
};

#ifdef _AC_DEBUG_MEMORY_
ac_async_buffer_t *_ac_async_buffer_init(const char *caller) {
  ac_async_buffer_t *p = (ac_async_buffer_t *)_ac_calloc_d(
      NULL, caller, sizeof(ac_async_buffer_t), false);
#else
ac_async_buffer_t *_ac_async_buffer_init() {
  ac_async_buffer_t *p =
      (ac_async_buffer_t *)ac_calloc(sizeof(ac_async_buffer_t));
#endif
  p->buffer = ac_buffer_init(1024 * 4);
  return p;
}

static int advance_mem_delimiter(ac_async_buffer_t *);

void ac_async_buffer_destroy(ac_async_buffer_t *p) {
  if (p) {
    ac_buffer_destroy(p->buffer);
    if (p->delayed_advance == advance_mem_delimiter && p->mem_delimiter) {
      ac_free(p->mem_delimiter);
    }
    ac_free(p);
  }
}

void ac_async_buffer_set_arg(ac_async_buffer_t *p, void *arg) {
  if (p) {
    p->arg = arg;
  }
}

void *ac_async_buffer_get_arg(ac_async_buffer_t *p) {
  return (p) ? p->arg : NULL;
}

static int advance_char_delimiter(ac_async_buffer_t *p) {
  p->chunk_start = NULL;
  if ((p->chunk_end = (char *)memchr(p->data_start, p->char_delimiter,
                                     p->data_end - p->data_start))) {
    if (ac_buffer_length(p->buffer)) {
      // partial data previously stored in buffer
      ac_buffer_append(p->buffer, p->data_start,
                         p->chunk_end - p->data_start);
      p->chunk_start = ac_buffer_data(p->buffer);
      p->chunk_end = p->chunk_start + ac_buffer_length(p->buffer);
      p->clear_buffer = 1;
    } else {
      // complete chunk available in this one
      p->chunk_start = p->data_start;
    }
    p->data_start += ac_async_buffer_data_length(p) + 1;
    if (p->data_start >= p->data_end) {
      p->data_start = p->data_end = NULL;
    }
  } else {
    ac_buffer_append(p->buffer, p->data_start, p->data_end - p->data_start);
    p->data_start = p->data_end = NULL;
  }
  return (p->chunk_start) ? 1 : 0;
}

static char *find_mem_delimiter(ac_async_buffer_t *p, char *mem,
                                char *end_mem) {
  return memmem(mem, end_mem - mem, p->mem_delimiter, p->mem_delimiter_length);
}

static int advance_mem_delimiter(ac_async_buffer_t *p) {
  p->chunk_start = p->chunk_end = NULL;
  char *delimiter_start;
  if (ac_buffer_length(p->buffer)) {
    /*  partial data stored from last chunk;
        backtrack and see if the delimiter
        is split between chunks.         */
    size_t prev_length = ac_buffer_length(p->buffer);
    size_t append_length = p->mem_delimiter_length - 1;
    if (p->data_end - p->data_start < append_length) {
      append_length = p->data_end - p->data_start;
    }
    ac_buffer_append(p->buffer, p->data_start, append_length);
    char *prev = ac_buffer_data(p->buffer);
    char *end_prev = prev + ac_buffer_length(p->buffer);
    if (ac_buffer_length(p->buffer) > p->mem_delimiter_length) {
      prev += ac_buffer_length(p->buffer) - (p->mem_delimiter_length + 1);
    }
    if ((delimiter_start = find_mem_delimiter(p, prev, end_prev))) {
      /*  full record is in the buffer already  */
      p->chunk_start = ac_buffer_data(p->buffer);
      p->chunk_end = delimiter_start;
      p->clear_buffer = 1;
      char *append_start = ac_buffer_data(p->buffer) + prev_length;
      p->data_start +=
          (delimiter_start - append_start) + p->mem_delimiter_length;
      if (p->data_start >= p->data_end) {
        p->data_start = p->data_end = NULL;
      }
    } else {
      ac_buffer_shrink_by(p->buffer, append_length);
      if ((delimiter_start =
               find_mem_delimiter(p, p->data_start, p->data_end))) {
        ac_buffer_append(p->buffer, p->data_start,
                           delimiter_start - p->data_start);
        p->chunk_start = ac_buffer_data(p->buffer);
        p->chunk_end = p->chunk_start + ac_buffer_length(p->buffer);
        p->clear_buffer = 1;
        p->data_start = delimiter_start + p->mem_delimiter_length;
      } else {
        ac_buffer_append(p->buffer, p->data_start,
                           p->data_end - p->data_start);
      }
    }
  } else {
    if ((delimiter_start = find_mem_delimiter(p, p->data_start, p->data_end))) {
      p->chunk_start = p->data_start;
      p->chunk_end = delimiter_start;
      p->data_start = delimiter_start + p->mem_delimiter_length;
    } else {
      ac_buffer_append(p->buffer, p->data_start, p->data_end - p->data_start);
    }
  }
  return (p->chunk_start) ? 1 : 0;
}

static int advance_bytes_delimiter(ac_async_buffer_t *p) {
  p->chunk_start = p->chunk_end = NULL;
  size_t buffered = ac_buffer_length(p->buffer);
  size_t needed = p->bytes - buffered;
  size_t available = p->data_end - p->data_start;
  if (available >= needed) {
    if (buffered) {
      ac_buffer_append(p->buffer, p->data_start, needed);
      p->chunk_start = ac_buffer_data(p->buffer);
      p->clear_buffer = 1;
    } else {
      p->chunk_start = p->data_start;
    }
    p->chunk_end = p->chunk_start + p->bytes;
    p->data_start += needed;
  } else {
    ac_buffer_append(p->buffer, p->data_start, p->data_end - p->data_start);
    p->data_start = p->data_end = NULL;
  }
  return (p->chunk_start) ? 1 : 0;
}

static void clear_buffer(ac_async_buffer_t *p) {
  if (p->clear_buffer) {
    p->clear_buffer = 0;
    ac_buffer_clear(p->buffer);
  }
}

void ac_async_buffer_clear(ac_async_buffer_t *p) {
  if (p) {
    ac_buffer_clear(p->buffer);
    p->data_start = p->data_end = NULL;
    p->chunk_start = p->chunk_end = NULL;
  }
}

int ac_async_buffer_advance_to_char(ac_async_buffer_t *p, char delimiter,
                                      ac_async_buffer_f cb) {
  int res = 0;
  if (p && cb) {
    clear_buffer(p);
    p->char_delimiter = delimiter;
    res = advance_char_delimiter(p);
    if (!res) {
      p->on_data_ready = cb;
      p->delayed_advance = advance_char_delimiter;
    }
  }
  return res;
}

int ac_async_buffer_advance_to_mem(ac_async_buffer_t *p,
                                     void *mem_delimiter, size_t len,
                                     ac_async_buffer_f cb) {
  int res = 0;
  if (p && mem_delimiter && len && cb) {
    clear_buffer(p);
    p->mem_delimiter = mem_delimiter;
    p->mem_delimiter_length = len;
    res = advance_mem_delimiter(p);
    if (!res) { //
      p->on_data_ready = cb;
      p->delayed_advance = advance_mem_delimiter;
      p->mem_delimiter = ac_malloc(len);
      memcpy(p->mem_delimiter, mem_delimiter, len);
    } else {
      p->mem_delimiter = NULL;
    }
  }
  return res;
}

int ac_async_buffer_advance_to_string(ac_async_buffer_t *p,
                                        char const *string_delimiter,
                                        ac_async_buffer_f cb) {
  int res = 0;
  if (p && string_delimiter && string_delimiter[0] && cb) {
    res = ac_async_buffer_advance_to_mem(p, (void *)string_delimiter,
                                           strlen(string_delimiter), cb);
  }
  return res;
}

int ac_async_buffer_advance_bytes(ac_async_buffer_t *p, size_t len,
                                    ac_async_buffer_f cb) {
  int res = 0;
  if (p && cb) {
    clear_buffer(p);
    if (len == 0) {
      res = 1;
    } else {
      p->bytes = len;
      res = advance_bytes_delimiter(p);
      if (!res) {
        p->on_data_ready = cb;
        p->delayed_advance = advance_bytes_delimiter;
      }
    }
  }
  return res;
}

char *ac_async_buffer_data(ac_async_buffer_t *p) {
  return (p) ? p->chunk_start : NULL;
}

size_t ac_async_buffer_data_length(ac_async_buffer_t *p) {
  return (p) ? (p->chunk_end - p->chunk_start) : 0;
}

void ac_async_buffer_parse(ac_async_buffer_t *p, void const *data,
                             size_t len) {
  if (p && data && len) {
    if (!p->delayed_advance) {
      fprintf(stderr, "You must advance BufferedReader until it returns false "
                      "before adding input.\n");
      abort();
    }
    p->data_start = (void *)data;
    p->data_end = p->data_start + len;
    if (p->delayed_advance(p)) {
      if (p->delayed_advance == advance_mem_delimiter && p->mem_delimiter) {
        ac_free(p->mem_delimiter);
        p->mem_delimiter = NULL;
      }
      p->delayed_advance = NULL;
      p->on_data_ready(p);
    }
  }
}
