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

#ifndef _ac_threaded_pipe_H
#define _ac_threaded_pipe_H

#include "ac_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ac_threaded_pipe_close_cb)(void *d);

struct ac_threaded_pipe_s;
typedef struct ac_threaded_pipe_s ac_threaded_pipe_t;

#ifdef _AC_MEMORY_CHECK_
#define ac_threaded_pipe_init(num_threads)                                     \
  _ac_threaded_pipe_init(num_threads, AC_FILE_LINE_MACRO("ac_threaded_pipe"))
ac_threaded_pipe_t *_ac_threaded_pipe_init(int num_threads, const char *caller);
#else
#define ac_threaded_pipe_init(num_threads) _ac_threaded_pipe_init(num_threads)
ac_threaded_pipe_t *_ac_threaded_pipe_init(int num_threads);
#endif

typedef void *(*ac_threaded_pipe_create_global_arg_cb)(void *update_arg,
                                                      void *old_global_arg);
typedef void (*ac_threaded_pipe_destroy_global_arg_cb)(void *update_arg,
                                                      void *global_arg);

void ac_threaded_pipe_set_global_arg(
    ac_threaded_pipe_t *s, void *arg,
    ac_threaded_pipe_destroy_global_arg_cb destroy_arg);

void ac_threaded_pipe_set_global_methods(
    ac_threaded_pipe_t *s, void *update_arg, size_t update_interval,
    ac_threaded_pipe_create_global_arg_cb create_arg,
    ac_threaded_pipe_destroy_global_arg_cb destroy_arg);

typedef void *(*ac_threaded_pipe_create_thread_arg_cb)(void *global_arg);
typedef void (*ac_threaded_pipe_clear_thread_arg_cb)(void *thread_arg);
typedef void (*ac_threaded_pipe_destroy_thread_arg_cb)(void *global_arg,
                                                      void *thread_arg);

void ac_threaded_pipe_set_thread_methods(
    ac_threaded_pipe_t *s, ac_threaded_pipe_create_thread_arg_cb create,
    ac_threaded_pipe_clear_thread_arg_cb clear,
    ac_threaded_pipe_destroy_thread_arg_cb destroy);

void ac_threaded_pipe_set_close_cb(ac_threaded_pipe_t *h,
                                   ac_threaded_pipe_close_cb cb, void *arg);

void ac_threaded_pipe_open(ac_threaded_pipe_t *h);

typedef void (*ac_threaded_pipe_cb)(void *global_arg, void *thread_arg,
                                   void *object, void *arg);

bool ac_threaded_pipe_write(ac_threaded_pipe_t *h, ac_threaded_pipe_cb cb,
                            void *object, void *arg);

void ac_threaded_pipe_close(ac_threaded_pipe_t *h);

#ifdef __cplusplus
}
#endif

#endif
