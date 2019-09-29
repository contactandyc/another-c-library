#ifndef _stla_threaded_pipe_H
#define _stla_threaded_pipe_H

#include "stla_common.h"

typedef void (*stla_threaded_pipe_close_f)(void *d);

struct stla_threaded_pipe_s;
typedef struct stla_threaded_pipe_s stla_threaded_pipe_t;

#ifdef _STLA_DEBUG_MEMORY_
#define stla_threaded_pipe_init(num_threads)                                   \
  _stla_threaded_pipe_init(num_threads,                                        \
                           STLA_FILE_LINE_MACRO("stla_threaded_pipe"))
stla_threaded_pipe_t *_stla_threaded_pipe_init(int num_threads,
                                               const char *caller);
#else
#define stla_threaded_pipe_init(num_threads)                                   \
  _stla_threaded_pipe_init(num_threads)
stla_threaded_pipe_t *_stla_threaded_pipe_init(int num_threads);
#endif

typedef void *(*stla_threaded_pipe_create_global_arg_f)(void *update_arg,
                                                        void *old_global_arg);
typedef void (*stla_threaded_pipe_destroy_global_arg_f)(void *update_arg,
                                                        void *global_arg);

void stla_threaded_pipe_set_global_arg(
    stla_threaded_pipe_t *s, void *arg,
    stla_threaded_pipe_destroy_global_arg_f destroy_arg);

void stla_threaded_pipe_set_global_methods(
    stla_threaded_pipe_t *s, void *update_arg, size_t update_interval,
    stla_threaded_pipe_create_global_arg_f create_arg,
    stla_threaded_pipe_destroy_global_arg_f destroy_arg);

typedef void *(*stla_threaded_pipe_create_thread_arg_f)(void *global_arg);
typedef void (*stla_threaded_pipe_clear_thread_arg_f)(void *thread_arg);
typedef void (*stla_threaded_pipe_destroy_thread_arg_f)(void *global_arg,
                                                        void *thread_arg);

void stla_threaded_pipe_set_thread_methods(
    stla_threaded_pipe_t *s, stla_threaded_pipe_create_thread_arg_f create,
    stla_threaded_pipe_clear_thread_arg_f clear,
    stla_threaded_pipe_destroy_thread_arg_f destroy);

void stla_threaded_pipe_set_close_cb(stla_threaded_pipe_t *h,
                                     stla_threaded_pipe_close_f cb, void *arg);

void stla_threaded_pipe_open(stla_threaded_pipe_t *h);

typedef void (*stla_threaded_pipe_f)(void *global_arg, void *thread_arg,
                                     void *object, void *arg);

bool stla_threaded_pipe_write(stla_threaded_pipe_t *h, stla_threaded_pipe_f cb,
                              void *object, void *arg);

void stla_threaded_pipe_close(stla_threaded_pipe_t *h);

#endif
