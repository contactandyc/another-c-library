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

#ifndef _acthreaded_pipe_H
#define _acthreaded_pipe_H

#include "accommon.h"

typedef void (*acthreaded_pipe_close_f)(void *d);

struct acthreaded_pipe_s;
typedef struct acthreaded_pipe_s acthreaded_pipe_t;

#ifdef _ACDEBUG_MEMORY_
#define acthreaded_pipe_init(num_threads)                                   \
  _acthreaded_pipe_init(num_threads,                                        \
                           ACFILE_LINE_MACRO("acthreaded_pipe"))
acthreaded_pipe_t *_acthreaded_pipe_init(int num_threads,
                                               const char *caller);
#else
#define acthreaded_pipe_init(num_threads)                                   \
  _acthreaded_pipe_init(num_threads)
acthreaded_pipe_t *_acthreaded_pipe_init(int num_threads);
#endif

typedef void *(*acthreaded_pipe_create_global_arg_f)(void *update_arg,
                                                        void *old_global_arg);
typedef void (*acthreaded_pipe_destroy_global_arg_f)(void *update_arg,
                                                        void *global_arg);

void acthreaded_pipe_set_global_arg(
    acthreaded_pipe_t *s, void *arg,
    acthreaded_pipe_destroy_global_arg_f destroy_arg);

void acthreaded_pipe_set_global_methods(
    acthreaded_pipe_t *s, void *update_arg, size_t update_interval,
    acthreaded_pipe_create_global_arg_f create_arg,
    acthreaded_pipe_destroy_global_arg_f destroy_arg);

typedef void *(*acthreaded_pipe_create_thread_arg_f)(void *global_arg);
typedef void (*acthreaded_pipe_clear_thread_arg_f)(void *thread_arg);
typedef void (*acthreaded_pipe_destroy_thread_arg_f)(void *global_arg,
                                                        void *thread_arg);

void acthreaded_pipe_set_thread_methods(
    acthreaded_pipe_t *s, acthreaded_pipe_create_thread_arg_f create,
    acthreaded_pipe_clear_thread_arg_f clear,
    acthreaded_pipe_destroy_thread_arg_f destroy);

void acthreaded_pipe_set_close_cb(acthreaded_pipe_t *h,
                                     acthreaded_pipe_close_f cb, void *arg);

void acthreaded_pipe_open(acthreaded_pipe_t *h);

typedef void (*acthreaded_pipe_f)(void *global_arg, void *thread_arg,
                                     void *object, void *arg);

bool acthreaded_pipe_write(acthreaded_pipe_t *h, acthreaded_pipe_f cb,
                              void *object, void *arg);

void acthreaded_pipe_close(acthreaded_pipe_t *h);

#endif
