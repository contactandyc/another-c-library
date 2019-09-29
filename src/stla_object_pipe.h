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
