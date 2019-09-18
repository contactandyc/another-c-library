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

#include "stla_allocator.h"
#include <pthread.h>
#include <string.h>

typedef struct stla_allocator_node_s {
  const char *caller;
  ssize_t length;
  struct stla_allocator_node_s *next;
  struct stla_allocator_node_s *previous;
  stla_allocator_t *a;
} stla_allocator_node_t;

struct stla_allocator_s;
typedef struct stla_allocator_s
  stla_allocator_t;

struct stla_allocator_s {
  stla_allocator_node_t *head;
  stla_allocator_node_t *tail;
  size_t total_bytes_allocated;
  size_t total_allocations;
  const char *logfile;
  bool thread_safe;
  pthread_t thread;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
  int done;
};

static void print_node(FILE *out, const char *caller, ssize_t len, stla_allocator_node_t *n ) {
  if(len >= 0)
    fprintf( out, "%s: %ld ", caller, len );
  else {
    stla_allocator_dump_t *d = (stla_allocator_dump_t *)(n+1);
    ssize_t num = -len;
    size_t length = num;
    d->dump(out, caller, d, length);
  }
}

static void _stla_dump_global_allocations(stla_allocator_t *a, FILE *out) {
  if(a->head) {
    fprintf(out, "%lu byte(s) allocated in %lu allocations (%lu byte(s) overhead)\n",
            a->total_bytes_allocated, a->total_allocations, a->total_allocations*sizeof(stla_allocator_node_t) );
    stla_allocator_node_t *n = a->head;
    while(n) {
      print_node(out, n->caller, n->length, n );
      fprintf( out, "\n");
      n = n->next;
    }
  }
}

void stla_dump_global_allocations(stla_allocator_t *a, FILE *out) {
  if(a->thread_safe)
    pthread_mutex_lock(&a->mutex);
  _stla_dump_global_allocations(a, out);
  if(a->thread_safe)
    pthread_mutex_unlock(&a->mutex);
}

stla_allocator_t *global_allocator = NULL;

void save_old_log(stla_allocator_t *a, size_t saves, char *tmp) {
  int num = 0;
  while(saves) {
    char *p = tmp;
    char *old_name = p;
    if(num) {
      sprintf(old_name, "%s.%d", a->logfile, num);
      p += strlen(old_name) + 1;
    }
    else
      old_name = (char *)a->logfile;
    num++;
    char *new_name = p;
    sprintf(new_name, "%s.%d", a->logfile, num );
    rename(old_name, new_name);
    saves >>= 1;
  }
}

void * dump_global_allocations_thread( void *arg ) {
  stla_allocator_t *a = (stla_allocator_t *)arg;
  // save every 60 seconds, back off now, 60 seconds ago, 240 seconds ago, 240*4 seconds ago
  size_t save = 0;
  struct timespec ts;

  char *tmp = (char *)malloc((strlen(a->logfile)*2)*100);
  int done = 0;
  while(!done) {
    save_old_log(a, save, tmp);
    pthread_mutex_lock(&a->mutex);
    FILE *out = fopen(a->logfile, "wb");
    _stla_dump_global_allocations(a, out);
    fclose(out);
    if(a->done)
      done = 1;
    else {
      clock_gettime(CLOCK_REALTIME, &ts);
      ts.tv_sec += _STLA_DEBUG_MEMORY_SPEED_;
      pthread_cond_timedwait(&a->cond, &a->mutex, &ts);
    }
    pthread_mutex_unlock(&a->mutex);
  }
  free(tmp);
  return NULL;
}

stla_allocator_t *stla_allocator_init(const char *filename, bool thread_safe) {
  stla_allocator_t *a = (stla_allocator_t *)malloc(sizeof(stla_allocator_t));
  a->head = NULL;
  a->tail = NULL;
  a->total_bytes_allocated = 0;
  a->total_allocations = 0;
  a->logfile = filename;
  a->done = 0;
  a->thread_safe = thread_safe;
  if(thread_safe) {
    if(filename) {
      pthread_cond_init(&a->cond, NULL);
      pthread_create(&(a->thread), NULL, dump_global_allocations_thread, a);
    }
    pthread_mutex_init(&a->mutex, NULL);
  }
  return a;
}

void stla_allocator_destroy(stla_allocator_t *a) {
  if(a->logfile && a->thread_safe) {
    /* broadcast that we are done to the main thread */
    pthread_mutex_lock(&a->mutex);
    a->done = 1;
    pthread_cond_signal(&a->cond);
    pthread_mutex_unlock(&a->mutex);
    pthread_join(a->thread, NULL);
  }
  else {
    if(a->logfile) {
      FILE *out = fopen(a->logfile, "wb");
      stla_dump_global_allocations(a, stderr);
      fclose(out);
    }
    else
      stla_dump_global_allocations(a, stderr);
  }
  free(a);
}

void myStartupFun (void) __attribute__ ((constructor));
void myCleanupFun (void) __attribute__ ((destructor));

void myStartupFun (void)
{
#ifdef _STLA_DEBUG_MEMORY_
  global_allocator = stla_allocator_init(_STLA_DEBUG_MEMORY_, true);
#else
  global_allocator = stla_allocator_init(NULL, true);
#endif
}

void myCleanupFun (void)
{
  stla_allocator_destroy(global_allocator);
}


void *_stla_malloc_d(stla_allocator_t *a, const char *caller, size_t len, bool custom ) {
  if(!len)
    return NULL;

  if(!a)
    a = global_allocator;

  stla_allocator_node_t *n = (stla_allocator_node_t *)malloc(sizeof(stla_allocator_node_t) + len);
  if(!n) {
    if(a->thread_safe)
      pthread_mutex_lock(&a->mutex);
    print_node(stderr, caller, len, NULL);
    fprintf( stderr, "malloc failed\n" );
    if(a->thread_safe)
      pthread_mutex_unlock(&a->mutex);
    abort();
  }
  ssize_t l = len;
  if(custom)
    l = -l;

  n->caller = caller;
  n->length = l;
  n->next = NULL;
  n->a = a;

  if(a->thread_safe)
    pthread_mutex_lock(&a->mutex);
  a->total_bytes_allocated += len;
  a->total_allocations++;
  n->previous = a->tail;
  if(n->previous)
    n->previous->next = n;
  else
    a->head = n;
  a->tail = n;
  if(a->thread_safe)
    pthread_mutex_unlock(&a->mutex);
  return (void *)(n+1);
}

void *_stla_calloc_d(stla_allocator_t *a, const char *caller, size_t len, bool custom ) {
  void *m = _stla_malloc_d(a, caller, len, custom);
  if(m)
    memset(m, 0, len);
  return m;
}

char *_stla_strdup_d(stla_allocator_t *a, const char *caller, const char *p) {
  size_t len = strlen(p)+1;
  char *m = (char *)_stla_malloc_d(a, caller, len, false);
  strcpy(m, p);
  return m;
}

static stla_allocator_node_t *get_stla_node(stla_allocator_t *a, const char *caller, void *p,
                                            const char *message ) {
  stla_allocator_node_t *n = (stla_allocator_node_t *)p;
  n--;
  if(n->a == a)
    return n;

  stla_allocator_node_t *n2 = a->head;
  char *c = (char *)p;
  stla_allocator_node_t *closest = NULL;
  size_t closest_abs_dist;
  ssize_t closest_dist;
  while(n2) {
    char *p2 = (char *)(n2+1);
    size_t dist = p2 < c ? c-p2 : p2-c;
    if(!closest || dist < closest_abs_dist) {
      closest_abs_dist = dist;
      closest_dist = c-p2;
      closest = n2;
    }
    n2 = n2->next;
  }
  if(a->thread_safe)
    pthread_mutex_lock(&a->mutex);
  if(closest) {
    print_node(stderr,
               closest->caller,
               closest->length,
               closest);
    fprintf(stderr, "is closest allocation and is %lu bytes %s of original allocation\n",
            closest_abs_dist, closest_dist < 0 ? "behind" : "ahead");
  }
  print_node(stderr, caller, 0, NULL);
  fprintf(stderr, "%s\n", message);
  if(a->thread_safe)
    pthread_mutex_unlock(&a->mutex);
  abort();
}


void *_stla_realloc_d(stla_allocator_t *a, const char *caller, void *p, size_t len, bool custom) {
  if(!a)
    a = global_allocator;
  if(!p)
    return _stla_malloc_d(a, caller, len, custom);

  stla_allocator_node_t *n = get_stla_node(a, caller, p, "stla_realloc is invalid (p is not allocated?)");

  void *m = _stla_malloc_d(a, caller, len, custom);
  size_t len2;
  if(n->length > 0)
    len2 = n->length;
  else {
    ssize_t num=-n->length;
    len2 = num;
  }
  if(len2 < len)
    memcpy(m, p, len2);
  else
    memcpy(m, p, len);

  _stla_free_d(a, caller, p);
  return m;
}


void _stla_free_d(stla_allocator_t *a, const char *caller, void *p) {
  if(!p)
    return;
  if(!a)
    a = global_allocator;
  stla_allocator_node_t *n = get_stla_node(a, caller, p, "stla_free is invalid (double free?)");
  if(a->thread_safe)
    pthread_mutex_lock(&a->mutex);
  if (n->previous)
    n->previous->next = n->next;
  else
    a->head = n->next;
  if (n->next)
    n->next->previous = n->previous;
  else
    a->tail = n->previous;
  a->total_allocations--;
  if(n->length > 0)
    a->total_bytes_allocated -= n->length;
  else
    a->total_bytes_allocated += n->length;
  if(a->thread_safe)
    pthread_mutex_unlock(&a->mutex);
  n->a--; // to try and protect against double free
  free(n);
}
