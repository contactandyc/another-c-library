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

#ifndef _acobject_pipe_H
#define _acobject_pipe_H

#include "accommon.h"

#include <uv.h>

typedef void (*acobject_pipe_f)(void *d, void *arg);
typedef void (*acobject_pipe_close_f)(void *d);

struct acobject_pipe_s;
typedef struct acobject_pipe_s acobject_pipe_t;

#ifdef _ACDEBUG_MEMORY_
#define acobject_pipe_open(loop, cb, arg)                                   \
  _acobject_pipe_open(loop, cb, arg,                                        \
                         ACFILE_LINE_MACRO("acobject_pipe"))
acobject_pipe_t *_acobject_pipe_open(uv_loop_t *loop,
                                           acobject_pipe_f cb, void *arg,
                                           const char *caller);
#else
#define acobject_pipe_open(loop, cb, arg)                                   \
  _acobject_pipe_open(loop, cb, arg)
acobject_pipe_t *_acobject_pipe_open(uv_loop_t *loop,
                                           acobject_pipe_f cb, void *arg);
#endif

void acobject_pipe_set_close_cb(acobject_pipe_t *h,
                                   acobject_pipe_close_f cb);

void acobject_pipe_write(acobject_pipe_t *h, void *object);
void acobject_pipe_close(acobject_pipe_t *h);

#endif
