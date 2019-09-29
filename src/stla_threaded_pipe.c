#include "stla_threaded_pipe.h"

#include "stla_allocator.h"

#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
  void *thread_arg;
  void *global_arg;
  pthread_t thread;
  stla_threaded_pipe_t *h;
  void *new_args[8];
} thread_data_t;

typedef struct {
  void *object;
  void *arg;
  stla_threaded_pipe_f cb;
} stla_threaded_pipe_object_t;

struct stla_threaded_pipe_s {
  int read_fd;
  int write_fd;
  stla_threaded_pipe_f cb;
  thread_data_t *threads;
  int num_threads;

  stla_threaded_pipe_close_f close_cb;
  void *close_arg;

  pid_t parent_pid;
  bool done;

  pthread_t update_thread;
  void *update_arg;
  size_t update_interval;
  time_t global_update_time;

  stla_threaded_pipe_create_global_arg_f create_global_arg;
  stla_threaded_pipe_destroy_global_arg_f destroy_global_arg;
  void *global_arg;

  stla_threaded_pipe_create_thread_arg_f create_thread_arg;
  stla_threaded_pipe_clear_thread_arg_f clear_thread_arg;
  stla_threaded_pipe_destroy_thread_arg_f destroy_thread_arg;
};

void *update_task(void *arg) {
  stla_threaded_pipe_t *h = (stla_threaded_pipe_t *)arg;
  while (true) {
    if (h->done || (getppid() != h->parent_pid))
      return NULL;
    for (size_t i = 0; i < h->update_interval; i++) {
      sleep(1);
      if (h->done || (getppid() != h->parent_pid))
        return NULL;
    }

    void *new_gbl = h->create_global_arg(h->update_arg, h->global_arg);
    if (new_gbl) {
      if (new_gbl != h->global_arg) {
        for (int i = 0; i < h->num_threads; i++) {
          thread_data_t *t = h->threads + i;
          void *new_thread_arg = t->thread_arg;
          if (h->create_thread_arg)
            new_thread_arg = h->create_thread_arg(new_gbl);
          t->new_args[0] = new_thread_arg;
          t->new_args[1] = NULL;
          t->new_args[2] = new_gbl;
          t->new_args[3] = new_gbl;
          t->new_args[4] = new_gbl;
          t->new_args[5] = new_gbl;
          t->new_args[6] = new_gbl;
          t->new_args[7] = new_gbl;
        }
        bool found_arg = true;
        while (found_arg) {
          found_arg = false;
          for (int i = 0; i < h->num_threads; i++) {
            thread_data_t *t = h->threads + i;
            bool found = false;
            for (int j = 2; j < 7; j++) {
              if (t->new_args[j]) {
                found = true;
                break;
              }
            }
            if (t->new_args[0] != t->new_args[1])
              found = true;
            if (!found) {
              if (t->new_args[0] && h->destroy_thread_arg)
                h->destroy_thread_arg(h->global_arg, t->new_args[0]);
              t->new_args[1] = NULL;
            } else
              found_arg = true;
          }
          if (found_arg)
            sleep(1);
        }
        if (h->global_arg && h->destroy_global_arg)
          h->destroy_global_arg(h->update_arg, h->global_arg);
        h->global_arg = new_gbl;
        h->global_update_time = time(NULL);
      }
    }
  }
}

void *do_task(void *arg) {
  thread_data_t *t = (thread_data_t *)arg;
  stla_threaded_pipe_t *h = t->h;
  int n;
  stla_threaded_pipe_object_t obj;

  while (true) {
    /* I think this is a safe way to avoid a mutex */
    if (t->new_args[7] != t->global_arg) {
      bool found = false;
      for (int i = 2; i < 7; i++) {
        if (t->new_args[i] != t->new_args[7]) {
          found = true;
          break;
        }
      }
      if (!found) {
        t->new_args[1] = t->thread_arg;
        t->thread_arg = t->new_args[0];
        t->global_arg = t->new_args[2];
        t->new_args[0] = t->thread_arg;
        t->new_args[2] = NULL;
        t->new_args[3] = NULL;
        t->new_args[4] = NULL;
        t->new_args[5] = NULL;
        t->new_args[6] = NULL;
      }
    }
    if (h->clear_thread_arg)
      h->clear_thread_arg(t->thread_arg);
    n = read(h->read_fd, &obj, sizeof(obj));
    if (n > 0) {
      if ((ssize_t)(obj.object) != -1) {
        obj.cb(t->global_arg, t->thread_arg, obj.object, obj.arg);
      } else {
        close(h->read_fd);
        close(h->write_fd);
        h->write_fd = -1;
        return NULL;
      }
    } else
      break;
  }
  return NULL;
}

#ifdef _STLA_DEBUG_MEMORY_
stla_threaded_pipe_t *_stla_threaded_pipe_init(int num_threads,
                                               const char *caller) {
  stla_threaded_pipe_t *h = (stla_threaded_pipe_t *)_stla_malloc_d(
      NULL, caller,
      sizeof(stla_threaded_pipe_t) + (sizeof(thread_data_t) * num_threads),
      false);
#else
stla_threaded_pipe_t *_stla_threaded_pipe_init(int num_threads) {
  stla_threaded_pipe_t *h = (stla_threaded_pipe_t *)stla_malloc(
      sizeof(stla_threaded_pipe_t) + (sizeof(thread_data_t) * num_threads));
#endif
  h->threads = (thread_data_t *)(h + 1);
  h->num_threads = num_threads;
  h->global_arg = NULL;
  h->destroy_global_arg = NULL;
  h->create_global_arg = NULL;
  h->update_interval = 0;
  h->update_arg = NULL;
  h->create_thread_arg = NULL;
  h->clear_thread_arg = NULL;
  h->destroy_thread_arg = NULL;
  h->cb = NULL;
  h->close_cb = NULL;
  h->parent_pid = getppid();
  h->done = false;
  return h;
}

void stla_threaded_pipe_set_global_arg(
    stla_threaded_pipe_t *s, void *arg,
    stla_threaded_pipe_destroy_global_arg_f destroy_arg) {
  s->global_arg = arg;
  s->destroy_global_arg = destroy_arg;
}

void stla_threaded_pipe_set_global_methods(
    stla_threaded_pipe_t *s, void *update_arg, size_t update_interval,
    stla_threaded_pipe_create_global_arg_f create_arg,
    stla_threaded_pipe_destroy_global_arg_f destroy_arg) {
  s->update_arg = update_arg;
  s->update_interval = update_interval;
  s->create_global_arg = create_arg;
  s->destroy_global_arg = destroy_arg;
  if (create_arg)
    s->global_arg = create_arg(update_arg, NULL);
}

void stla_threaded_pipe_set_thread_methods(
    stla_threaded_pipe_t *s, stla_threaded_pipe_create_thread_arg_f create,
    stla_threaded_pipe_clear_thread_arg_f clear,
    stla_threaded_pipe_destroy_thread_arg_f destroy) {
  s->create_thread_arg = create;
  s->clear_thread_arg = clear;
  s->destroy_thread_arg = destroy;
}

void stla_threaded_pipe_set_close_cb(stla_threaded_pipe_t *h,
                                     stla_threaded_pipe_close_f cb, void *arg) {
  h->close_cb = cb;
  h->close_arg = arg;
}

bool stla_threaded_pipe_write(stla_threaded_pipe_t *h, stla_threaded_pipe_f cb,
                              void *object, void *arg) {
  if (h->write_fd == -1 || (ssize_t)(object) == -1 ||
      (ssize_t)(object) == -2) // protect -1 from getting through
    return false;
  stla_threaded_pipe_object_t o;
  o.object = object;
  o.arg = arg;
  o.cb = cb;
  int n = write(h->write_fd, &o, sizeof(o));
  (void)n;
  return true;
}

void stla_threaded_pipe_close(stla_threaded_pipe_t *h) {
  if (h->write_fd == -1)
    return;

  stla_threaded_pipe_object_t o;
  ssize_t v = -1;
  o.object = (void *)(v);
  o.arg = NULL;
  o.cb = NULL;
  int n = write(h->write_fd, &o, sizeof(o));
  (void)n;
  h->done = true;
  if (h->update_interval)
    pthread_join(h->update_thread, NULL);
  for (int i = 0; i < h->num_threads; i++) {
    pthread_join(h->threads[i].thread, NULL);
    if (h->destroy_thread_arg && h->threads[i].thread_arg)
      h->destroy_thread_arg(h->threads[i].global_arg, h->threads[i].thread_arg);
  }
  if (h->destroy_global_arg)
    h->destroy_global_arg(h->update_arg, h->global_arg);
  if (h->close_cb)
    h->close_cb(h->close_arg);
  stla_free(h);
}

static void setnonblock(int fd) {
  int flags = fcntl(fd, F_GETFL);
  if (flags < 0)
    abort();

  flags |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flags) < 0)
    abort();
}

void stla_threaded_pipe_open(stla_threaded_pipe_t *h) {
  int fds[2];
  int n = pipe(fds);
  if (n < 0)
    abort();

  setnonblock(fds[1]);

  h->read_fd = fds[0];
  h->write_fd = fds[1];
  void *global_arg = h->global_arg;
  for (int i = 0; i < h->num_threads; i++) {
    thread_data_t *t = h->threads + i;
    void *thread_arg = NULL;
    if (h->create_thread_arg)
      thread_arg = h->create_thread_arg(global_arg);

    t->thread_arg = thread_arg;
    t->global_arg = global_arg;
    t->new_args[0] = thread_arg;
    t->new_args[1] = thread_arg;
    t->new_args[2] = NULL;
    t->new_args[3] = NULL;
    t->new_args[4] = NULL;
    t->new_args[5] = NULL;
    t->new_args[6] = NULL;
    t->new_args[7] = global_arg;
    t->h = h;
    pthread_create(&h->threads[i].thread, NULL, do_task, t);
  }
  if (h->update_interval) {
    pthread_create(&h->update_thread, NULL, update_task, h);
  }
}
