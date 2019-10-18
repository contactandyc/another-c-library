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

#ifndef _acasync_buffer_H
#define _acasync_buffer_H

#include <stddef.h>

struct acasync_buffer_s;
typedef struct acasync_buffer_s acasync_buffer_t;

typedef void (*acasync_buffer_f)(acasync_buffer_t *);

/*  Init/clear/destroy functionality  */
#ifdef _ACDEBUG_MEMORY_
#define acasync_buffer_init()                                               \
  _acasync_buffer_init(ACFILE_LINE_MACRO("acasync_buffer"))
acasync_buffer_t *_acasync_buffer_init(const char *caller);
#else
#define acasync_buffer_init() _acasync_buffer_init()
acasync_buffer_t *_acasync_buffer_init();
#endif

void acasync_buffer_clear(acasync_buffer_t *);
void acasync_buffer_destroy(acasync_buffer_t *);

void acasync_buffer_set_arg(acasync_buffer_t *, void *);
void *acasync_buffer_get_arg(acasync_buffer_t *);

/*  The advance functions return 1 for success and 0 for failure  */
int acasync_buffer_advance_to_char(acasync_buffer_t *, char delimiter,
                                      acasync_buffer_f);
int acasync_buffer_advance_to_string(acasync_buffer_t *,
                                        char const *delimiter,
                                        acasync_buffer_f);
int acasync_buffer_advance_to_mem(acasync_buffer_t *, void *delimiter,
                                     size_t delimiter_length,
                                     acasync_buffer_f);
int acasync_buffer_advance_bytes(acasync_buffer_t *, size_t bytes,
                                    acasync_buffer_f);

/*  Get the data for the current advance action  */
char *acasync_buffer_data(acasync_buffer_t *);
/*  Get the length of the current advance action.
    This may be 0 if the delimiter was at the 0th position.  */
size_t acasync_buffer_data_length(acasync_buffer_t *);

void acasync_buffer_parse(acasync_buffer_t *, void const *data,
                             size_t data_length);
#endif
