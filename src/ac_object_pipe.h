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

#ifndef _ac_object_pipe_H
#define _ac_object_pipe_H

#include "ac_common.h"

#include <uv.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ac_object_pipe_cb)(void *d, void *arg);
typedef void (*ac_object_pipe_close_cb)(void *d);

struct ac_object_pipe_s;
typedef struct ac_object_pipe_s ac_object_pipe_t;

#ifdef _AC_DEBUG_MEMORY_
#define ac_object_pipe_open(loop, cb, arg)                                     \
  _ac_object_pipe_open(loop, cb, arg, AC_FILE_LINE_MACRO("ac_object_pipe"))
ac_object_pipe_t *_ac_object_pipe_open(uv_loop_t *loop, ac_object_pipe_cb cb,
                                       void *arg, const char *caller);
#else
#define ac_object_pipe_open(loop, cb, arg) _ac_object_pipe_open(loop, cb, arg)
ac_object_pipe_t *_ac_object_pipe_open(uv_loop_t *loop, ac_object_pipe_cb cb,
                                       void *arg);
#endif

void ac_object_pipe_set_close_cb(ac_object_pipe_t *h,
                                 ac_object_pipe_close_cb cb);

void ac_object_pipe_write(ac_object_pipe_t *h, void *object);
void ac_object_pipe_close(ac_object_pipe_t *h);

#ifdef __cplusplus
}
#endif

#endif
