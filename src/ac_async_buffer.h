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

#ifndef _ac_async_buffer_H
#define _ac_async_buffer_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ac_async_buffer_s;
typedef struct ac_async_buffer_s ac_async_buffer_t;

typedef void (*ac_async_buffer_f)(ac_async_buffer_t *);

/*  Init/clear/destroy functionality  */
#ifdef _AC_DEBUG_MEMORY_
#define ac_async_buffer_init()                                                 \
  _ac_async_buffer_init(AC_FILE_LINE_MACRO("ac_async_buffer"))
ac_async_buffer_t *_ac_async_buffer_init(const char *caller);
#else
#define ac_async_buffer_init() _ac_async_buffer_init()
ac_async_buffer_t *_ac_async_buffer_init();
#endif

void ac_async_buffer_clear(ac_async_buffer_t *);
void ac_async_buffer_destroy(ac_async_buffer_t *);

void ac_async_buffer_set_arg(ac_async_buffer_t *, void *);
void *ac_async_buffer_get_arg(ac_async_buffer_t *);

/*  The advance functions return 1 for success and 0 for failure  */
int ac_async_buffer_advance_to_char(ac_async_buffer_t *, char delimiter,
                                    ac_async_buffer_f);
int ac_async_buffer_advance_to_string(ac_async_buffer_t *,
                                      char const *delimiter, ac_async_buffer_f);
int ac_async_buffer_advance_to_mem(ac_async_buffer_t *, void *delimiter,
                                   size_t delimiter_length, ac_async_buffer_f);
int ac_async_buffer_advance_bytes(ac_async_buffer_t *, size_t bytes,
                                  ac_async_buffer_f);

/*  Get the data for the current advance action  */
char *ac_async_buffer_data(ac_async_buffer_t *);
/*  Get the length of the current advance action.
    This may be 0 if the delimiter was at the 0th position.  */
size_t ac_async_buffer_data_length(ac_async_buffer_t *);

void ac_async_buffer_parse(ac_async_buffer_t *, void const *data,
                           size_t data_length);

#ifdef __cplusplus
}
#endif

#endif
