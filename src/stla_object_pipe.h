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

#ifndef _stla_object_pipe_H
#define _stla_object_pipe_H

#include "stla_common.h"

#include <uv.h>

typedef void (*stla_object_pipe_f)(void *d, void *arg);
typedef void (*stla_object_pipe_close_f)(void *d);

struct stla_object_pipe_s;
typedef struct stla_object_pipe_s stla_object_pipe_t;

#ifdef _STLA_DEBUG_MEMORY_
#define stla_object_pipe_open(loop, cb, arg)                                   \
  _stla_object_pipe_open(loop, cb, arg,                                        \
                         STLA_FILE_LINE_MACRO("stla_object_pipe"))
stla_object_pipe_t *_stla_object_pipe_open(uv_loop_t *loop,
                                           stla_object_pipe_f cb, void *arg,
                                           const char *caller);
#else
#define stla_object_pipe_open(loop, cb, arg)                                   \
  _stla_object_pipe_open(loop, cb, arg)
stla_object_pipe_t *_stla_object_pipe_open(uv_loop_t *loop,
                                           stla_object_pipe_f cb, void *arg);
#endif

void stla_object_pipe_set_close_cb(stla_object_pipe_t *h,
                                   stla_object_pipe_close_f cb);

void stla_object_pipe_write(stla_object_pipe_t *h, void *object);
void stla_object_pipe_close(stla_object_pipe_t *h);

#endif
