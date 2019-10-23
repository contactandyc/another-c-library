#include <uv.h>

#include "ac_buffer.h"
#include "ac_object_pipe.h"
#include "ac_pool.h"
#include "ac_threaded_pipe.h"

#define max_objects 50
#define buffer_size 100
#define num_threads 6
#define num_writer_threads 3
#define repeat_file 5

/*
  This will read files a line at a time and put the lines into a buffer (which
  is part of the object structure below).

  1. Read a line
  2. Get an object from the pool of objects so that the buffer can be filled in
  3. Write the object to the threaded_pipe
  4. One of the threads will pick up the write and uppercase the contents of
     the buffer.
  5. The thread will write the object to the object_pipe.
  6. One thread is listens to the object_pipe and will write the upper cased
     contents to "sample.out"
  7. The thread listening to the object pipe will return the object to the pool
     of objects and broadcast that there is an object if necessary to the
     threads which are reading the input file.
*/
typedef struct object_s {
  ac_buffer_t *bh;
  struct object_s *next;
} object_t;

object_t *create_objects() {
  object_t *res = NULL;
  for (int i = 0; i < max_objects; i++) {
    object_t *o = (object_t *)ac_malloc(sizeof(object_t));
    o->bh = ac_buffer_init(buffer_size);
    o->next = res;
    res = o;
  }
  return res;
}

object_t *objects;

typedef struct {
  ac_object_pipe_t *object_pipe;
  FILE *out;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
} global_arg_t;

typedef struct {
  ac_pool_t *pool;
} thread_arg_t;

void destroy_global_arg(void *update_arg, void *global_arg) {
  global_arg_t *g = (global_arg_t *)global_arg;
  ac_object_pipe_close(g->object_pipe);
}

void close_file(void *arg) {
  global_arg_t *g = (global_arg_t *)arg;
  fclose(g->out);
  ac_free(g);
}

void free_objects() {
  object_t *o = objects;
  while (o) {
    object_t *next = o->next;
    ac_buffer_destroy(o->bh);
    ac_free(o);
    o = next;
  }
}

void write_object(void *arg, void *obj) {
  global_arg_t *g = (global_arg_t *)arg;
  object_t *t = (object_t *)obj;
  fprintf(g->out, "%s\n", ac_buffer_data(t->bh));
  pthread_mutex_lock(&g->mutex);
  t->next = objects;
  objects = t;
  if (!objects->next)
    pthread_cond_broadcast(&g->cond);
  pthread_mutex_unlock(&g->mutex);
}

object_t *get_object(global_arg_t *g) {
  pthread_mutex_lock(&g->mutex);
  object_t *o = objects;
  while (!o) {
    pthread_cond_wait(&g->cond, &g->mutex);
    o = objects;
  }
  objects = o->next;
  pthread_mutex_unlock(&g->mutex);
  return o;
}

void *create_global_arg(void *update_arg) {
  uv_loop_t *loop = (uv_loop_t *)update_arg;
  global_arg_t *g = (global_arg_t *)ac_malloc(sizeof(*g));
  g->out = fopen("sample.out", "wb");
  g->object_pipe = ac_object_pipe_open(loop, write_object, g);
  pthread_mutex_init(&g->mutex, NULL);
  pthread_cond_init(&g->cond, NULL);
  ac_object_pipe_set_close_cb(g->object_pipe, close_file);
  return g;
}

void *create_thread_arg(void *global_arg) {
  // global_arg_t *g = (global_arg_t *)global_arg;
  thread_arg_t *t = (thread_arg_t *)ac_malloc(sizeof(*t));
  t->pool = ac_pool_init(100);
  return t;
}

void clear_thread_arg(void *thread_arg) {
  thread_arg_t *t = (thread_arg_t *)thread_arg;
  ac_pool_clear(t->pool);
}

void destroy_thread_arg(void *global_arg, void *thread_arg) {
  thread_arg_t *t = (thread_arg_t *)thread_arg;
  ac_pool_destroy(t->pool);
  ac_free(t);
}

void do_work(void *global_arg, void *thread_arg, void *object, void *arg) {
  global_arg_t *g = (global_arg_t *)global_arg;
  // thread_arg_t *t = (thread_arg_t *)thread_arg;
  object_t *o = (object_t *)object;
  char *p = ac_buffer_data(o->bh);
  char *ep = p + ac_buffer_length(o->bh);
  while (p < ep) {
    if (*p >= 'a' && *p <= 'z')
      *p = *p - 'a' + 'A';
    p++;
  }
  ac_object_pipe_write(g->object_pipe, object);
}

void *run_uv_loop(void *arg) {
  uv_loop_t *loop = (uv_loop_t *)arg;
  uv_run(loop, UV_RUN_DEFAULT);
  return NULL;
}

typedef struct {
  global_arg_t *g;
  char *filename;
  ac_threaded_pipe_t *threaded_pipe;
} writer_thread_t;

void *writer_thread(void *arg) {
  writer_thread_t *w = (writer_thread_t *)arg;
  char str[1000];
  for (int i = 0; i < repeat_file; i++) {
    FILE *in = fopen(w->filename, "rb");
    while (fgets(str, 999, in)) {
      object_t *o = get_object(w->g);
      ac_buffer_sets(o->bh, str);
      ac_threaded_pipe_write(w->threaded_pipe, do_work, o, NULL);
    }
    fclose(in);
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  printf("Demo illustrating how to use pipes to communicate between threads\n");
  printf("This will send each line of a file across a pipe using multiple\n");
  printf(
      "threads and then have one of multiple threads pick up the line and\n");
  printf(
      "uppercase it.  Once that is done, the line is sent back to a single\n");
  printf("writer thread which writes to sample.out.\n");

  objects = create_objects();

  ac_threaded_pipe_t *threaded_pipe = ac_threaded_pipe_init(num_threads);
  uv_loop_t *loop = uv_default_loop();
  void *global_arg = create_global_arg(loop);
  ac_threaded_pipe_set_global_arg(threaded_pipe, global_arg, NULL);
  ac_threaded_pipe_set_thread_methods(threaded_pipe, create_thread_arg,
                                        clear_thread_arg, destroy_thread_arg);

  ac_threaded_pipe_open(threaded_pipe);
  pthread_t uv_thread;
  pthread_create(&uv_thread, NULL, run_uv_loop, loop);
  writer_thread_t *w = (writer_thread_t *)ac_malloc(sizeof(writer_thread_t));
  w->g = (global_arg_t *)global_arg;
  w->filename = argv[1];
  w->threaded_pipe = threaded_pipe;
  pthread_t writer_threads[num_writer_threads];
  for (int i = 0; i < num_writer_threads; i++)
    pthread_create(writer_threads + i, NULL, writer_thread, w);
  for (int i = 0; i < num_writer_threads; i++)
    pthread_join(writer_threads[i], NULL);
  ac_free(w);
  ac_threaded_pipe_close(threaded_pipe);
  destroy_global_arg(NULL, global_arg);
  pthread_join(uv_thread, NULL);
  free_objects();
  return 0;
}
