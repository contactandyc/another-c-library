#include "ac_schedule.h"

#include "ac_allocator.h"
#include "ac_io.h"
#include "ac_map.h"

#include <math.h>
#include <pthread.h>
#include <unistd.h>

struct ac_task_state_link_s;
typedef struct ac_task_state_link_s ac_task_state_link_t;

struct ac_task_state_s;
typedef struct ac_task_state_s ac_task_state_t;

struct ac_task_state_link_s {
  bool waiting_on_others;
  ac_task_t *task;
  time_t ack_time;
  ac_task_state_link_t *next;
  ac_task_state_link_t *previous;
  time_t completed;
};

struct ac_task_state_s {
  ac_task_state_link_t *completed_tasks;
  ac_task_state_link_t *available_tasks;
  ac_task_state_link_t *tasks_to_finish;
};

struct ac_schedule_thread_s;
typedef struct ac_schedule_thread_s ac_schedule_thread_t;

struct ac_schedule_thread_s {
  pthread_t thread;
  ac_schedule_t *scheduler;
  size_t thread_id;
  size_t partition;
};

struct ac_schedule_s {
  ac_map_t *task_root;
  ac_pool_t *pool;
  ac_pool_t *tmp_pool;

  char *ack_dir;
  char *task_dir;

  size_t num_available;
  size_t num_tasks_to_run;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  size_t num_running;

  ac_schedule_thread_t *threads;
  bool started_threads;

  ac_task_part_f on_complete;
  bool done;

  size_t num_partitions;
  ac_task_state_t *state;

  ac_task_t *head;
  ac_task_t *tail;

  size_t ram;
  size_t cpus;
  size_t disk_space;
};

struct ac_task_s {
  ac_map_t node;

  char *task_name;
  size_t num_partitions;

  bool do_nothing;
  bool run_everytime;

  ac_schedule_t *scheduler;

  ac_task_f setup;
  ac_task_part_f runner;

  ac_task_input_t *inputs;
  ac_task_output_t *outputs;
  ac_task_link_t *dependencies;
  ac_task_link_t *partial_dependencies;
  ac_task_link_t *reverse_dependencies;
  ac_task_link_t *reverse_partial_dependencies;

  void *global_arg;
  size_t global_size;

  void *local_arg;
  size_t local_size;

  ac_task_state_link_t *state_linkage;

  ac_task_t *next;
};

ac_task_output_t *ac_task_get_output(ac_task_part_t *tp, size_t pos) {
  ac_task_output_t *r = tp->task->outputs;
  while (r && pos) {
    pos--;
    r = r->next;
  }
  if (pos)
    return NULL;
  return r;
}

ac_task_input_t *ac_task_get_input(ac_task_part_t *tp, size_t pos) {
  ac_task_input_t *r = tp->task->inputs;
  while (r && pos) {
    pos--;
    r = r->next;
  }
  if (pos)
    return NULL;
  return r;
}

int compare_task_for_find(const char *key, const ac_task_t *node) {
  return strcmp(key, node->task_name);
}

int compare_task_for_insert(const ac_task_t *a, const ac_task_t *b) {
  return strcmp(a->task_name, b->task_name);
}

static ac_map_find_m(_task_find, char, ac_task_t, compare_task_for_find);
static ac_map_insert_m(_task_insert, ac_task_t, compare_task_for_insert);

static ac_task_t *find_task(ac_schedule_t *h, const char *task_name) {
  return _task_find(task_name, h->task_root);
}

static ac_task_state_link_t *available_tasks(ac_schedule_t *h,
                                             size_t partition) {
  return h->state[partition].available_tasks;
}

static ac_task_state_link_t *completed_tasks(ac_schedule_t *h,
                                             size_t partition) {
  return h->state[partition].completed_tasks;
}

static ac_task_state_link_t *tasks_to_finish(ac_schedule_t *h,
                                             size_t partition) {
  return h->state[partition].tasks_to_finish;
}

ac_schedule_t *ac_schedule_init(size_t num_partitions, size_t cpus, size_t ram,
                                size_t disk_space) {
  ac_pool_t *pool = ac_pool_init(32768);
  ac_schedule_t *h = (ac_schedule_t *)ac_pool_calloc(
      pool, sizeof(ac_schedule_t) + (num_partitions * sizeof(ac_task_state_t)));

  pthread_mutex_init(&(h->mutex), NULL);
  pthread_cond_init(&(h->cond), NULL);

  h->threads = (ac_schedule_thread_t *)ac_pool_calloc(
      pool, sizeof(ac_schedule_thread_t) * cpus);
  for (size_t i = 0; i < cpus; i++) {
    ac_schedule_thread_t *a = h->threads + i;
    a->thread_id = i;
    a->scheduler = h;
  }
  h->started_threads = false;
  h->num_partitions = num_partitions;
  h->state = (ac_task_state_t *)(h + 1);
  h->ram = ram * 1024;
  h->cpus = cpus;
  h->disk_space = disk_space * 1024;
  h->pool = pool;
  h->tmp_pool = ac_pool_init(4096);
  return h;
}

void ac_schedule_ack_dir(ac_schedule_t *h, const char *ack_dir) {
  h->ack_dir = ac_pool_strdup(h->pool, ack_dir);
}

void ac_schedule_task_dir(ac_schedule_t *h, const char *task_dir) {
  h->task_dir = ac_pool_strdup(h->pool, task_dir);
}

void ac_schedule_destroy(ac_schedule_t *h) {
  ac_pool_destroy(h->tmp_pool);
  ac_pool_t *pool = h->pool;
  ac_pool_destroy(pool);
}

void print_state_link(ac_task_state_link_t *tasks) {
  ac_task_state_link_t *n = tasks;
  while (n) {
    printf(" %s", n->task->task_name);
    n = n->next;
  }
  printf("\n");
}

void ac_schedule_print(ac_schedule_t *h) {
  printf("%lu partitions\n", h->num_partitions);
  printf("%lu ram\n", h->ram);
  printf("%lu cpus\n", h->cpus);
  printf("%lu disk space\n", h->disk_space);

  for (size_t i = 0; i < h->num_partitions; i++) {
    printf("Completed[%lu]: ", i);
    print_state_link(h->state[i].completed_tasks);
    printf("Available[%lu]: ", i);
    print_state_link(h->state[i].available_tasks);
    printf("Waiting[%lu]: ", i);
    print_state_link(h->state[i].tasks_to_finish);
  }
}

static void unlink_state(ac_schedule_t *h, ac_task_state_link_t *state,
                         size_t partition) {
  if (state->previous) {
    state->previous->next = state->next;
    if (state->next)
      state->next->previous = state->previous;
  } else {
    ac_task_state_link_t **root = &(h->state[partition].available_tasks);
    if (state->waiting_on_others)
      root = &(h->state[partition].tasks_to_finish);
    else if (state->completed)
      root = &(h->state[partition].completed_tasks);

    *root = state->next;
    if (state->next)
      state->next->previous = NULL;
  }
  state->next = state->previous = NULL;
  if (!state->waiting_on_others && !state->completed)
    h->num_available--;
  else if (state->waiting_on_others)
    h->num_tasks_to_run--;
}

static void link_state(ac_schedule_t *h, ac_task_state_link_t *state,
                       size_t partition) {
  ac_task_state_link_t **root = &(h->state[partition].available_tasks);
  if (state->waiting_on_others)
    root = &(h->state[partition].tasks_to_finish);
  else if (state->completed)
    root = &(h->state[partition].completed_tasks);

  state->next = *root;
  state->previous = NULL;
  if (*root)
    (*root)->previous = state;

  *root = state;
  if (!state->waiting_on_others && !state->completed)
    h->num_available++;
  else if (state->waiting_on_others)
    h->num_tasks_to_run++;
}

static void mark_task_complete(ac_task_state_link_t *state_link,
                               size_t partition, time_t when);
static bool is_dependencies_complete(ac_task_t *task, size_t partition);

static ac_task_part_t *take_task(ac_schedule_t *scheduler, ac_pool_t *pool,
                                 ac_task_t *task, size_t partition) {
  ac_task_state_link_t *avail = available_tasks(scheduler, partition);
  while (avail) {
    if (!task || avail->task == task) {
      unlink_state(scheduler, avail, partition);
      ac_task_part_t *tp =
          (ac_task_part_t *)ac_pool_calloc(pool, sizeof(ac_task_part_t));
      tp->pool = pool;
      tp->task = avail->task;
      tp->partition = partition;
      tp->num_partitions = tp->task->num_partitions;
      tp->ack_time = -1;
      tp->__link = avail;
      return tp;
    }
    avail = avail->next;
  }
  return NULL;
}

static void _ac_task_input(ac_task_t *task, const char *name,
                           ac_task_output_t *src, double pct,
                           ac_task_check_input_f check) {
  ac_schedule_t *scheduler = task->scheduler;
  ac_pool_t *pool = scheduler->pool;

  ac_task_input_t *ti = (ac_task_input_t *)ac_pool_calloc(pool, sizeof(*ti));
  ti->name = ac_pool_strdup(pool, name);
  ti->ram_pct = pct;
  ti->task = task;
  ti->src = src;
  ti->check = check;
  ti->next = task->inputs;
  task->inputs = ti;
}

bool check_split(ac_task_part_t *tp, ac_task_input_t *inp, time_t ack_time) {
  // check if input files are newer than ack file
  size_t num_partitions = inp->src->task->num_partitions;
  for (size_t i = 0; i < num_partitions; i++) {
    const char *filename = ac_task_input_name(tp, inp, i);
    if (ac_io_modified(filename) > ack_time)
      return true;
  }
  return false;
}

bool check_first(ac_task_part_t *tp, ac_task_input_t *inp, time_t ack_time) {
  // check if input files are newer than ack file
  const char *filename = ac_task_input_name(tp, inp, 0);
  if (ac_io_modified(filename) > ack_time)
    return true;
  return false;
}

bool check_partition(ac_task_part_t *tp, ac_task_input_t *inp,
                     time_t ack_time) {
  // check if input files are newer than ack file
  const char *filename = ac_task_input_name(tp, inp, tp->partition);
  if (ac_io_modified(filename) > ack_time)
    return true;
  return false;
}

ac_task_link_t *get_task_list(ac_task_t *task, const char *name,
                              ac_schedule_t *h, const char *destinations) {
  if (!destinations)
    return NULL;
  if (strchr(destinations, '|')) {
    ac_task_link_t *r = NULL;
    ac_pool_clear(h->tmp_pool);
    char **dep = ac_pool_split2(h->tmp_pool, NULL, '|', destinations);
    for (size_t i = 0; dep[i] != NULL; i++) {
      ac_task_t *node = _task_find(dep[i], h->task_root);
      if (!node) {
        printf("%s is not a valid task (called from %s:%s)\n", dep[i],
               task->task_name, name);
        abort();
      }
      ac_task_link_t *tl = ac_pool_alloc(h->pool, sizeof(*tl));
      tl->task = node;
      tl->next = r;
      r = tl;
    }
    return r;
  } else {
    ac_task_t *node = _task_find(destinations, h->task_root);
    if (!node) {
      printf("%s is not a valid task (called from %s:%s)\n", destinations,
             task->task_name, name);
      abort();
    }

    ac_task_link_t *tl = ac_pool_alloc(h->pool, sizeof(*tl));
    tl->task = node;
    tl->next = NULL;
    return tl;
  }
}

void ac_task_output(ac_task_t *task, const char *name, const char *destinations,
                    double out_ram_pct, double in_ram_pct, size_t flags) {
  ac_schedule_t *scheduler = task->scheduler;
  ac_pool_t *pool = scheduler->pool;

  ac_task_output_t *to = (ac_task_output_t *)ac_pool_calloc(pool, sizeof(*to));
  to->name = ac_pool_strdup(pool, name);
  to->task = task;
  to->ram_pct = out_ram_pct;
  to->destinations = get_task_list(task, name, scheduler, destinations);
  to->flags = flags;
  to->num_partitions = task->num_partitions;
  to->next = task->outputs;
  task->outputs = to;

  ac_task_link_t *n = to->destinations;
  while (n) {
    ac_task_check_input_f check = NULL;
    if (flags & AC_OUTPUT_SPLIT)
      check = check_split;
    else if (flags & AC_OUTPUT_USE_FIRST)
      check = check_first;
    else
      check = check_partition;
    _ac_task_input(n->task, name, to, in_ram_pct, check);
    n = n->next;
  }
}

void ac_task_input_files(ac_task_t *task, const char *name, double pct,
                         ac_task_check_input_f check) {
  _ac_task_input(task, name, NULL, pct, check);
}

static void schedule_setup(ac_schedule_t *h) {
  if (!h->task_dir)
    h->task_dir = (char *)"tasks";
  if (!h->ack_dir)
    h->ack_dir = ac_pool_strdupf(h->pool, "%s/ack", h->task_dir);

  ac_buffer_t *bh = ac_buffer_init(100);
  ac_io_make_directory(h->ack_dir);
  ac_task_t *n = h->head;
  while (n) {
    if (n->setup)
      n->setup(n);
    for (size_t i = 0; i < n->num_partitions; i++) {
      ac_buffer_setf(bh, "%s/%s_%lu", h->task_dir, n->task_name, i);
      ac_io_make_directory(ac_buffer_data(bh));
    }
    n = n->next;
  }
  ac_buffer_destroy(bh);

  time_t current_time = time(NULL);
  n = h->head;
  while (n) {
    if (n->do_nothing) {
      for (size_t i = 0; i < n->num_partitions; i++) {
        if (is_dependencies_complete(n, i)) {
          ac_pool_clear(h->tmp_pool);
          ac_task_part_t *tp = take_task(h, h->tmp_pool, n, i);
          if (tp)
            mark_task_complete(tp->__link, i, current_time);
        }
      }
    }
    n = n->next;
  }
  ac_pool_clear(h->tmp_pool);
}

static bool task_available(ac_task_t *task, size_t partition) {
  ac_schedule_t *scheduler = task->scheduler;
  ac_task_state_link_t *avail = available_tasks(scheduler, partition);
  while (avail) {
    if (avail->task == task)
      return true;
    avail = avail->next;
  }
  return false;
}

static void write_ack(ac_task_part_t *tp) {
  char *filename =
      ac_pool_strdupf(tp->pool, "%s/%s_%lu", tp->task->scheduler->ack_dir,
                      tp->task->task_name, tp->partition);
  // printf("%s\n", filename);
  FILE *out = fopen(filename, "wb");
  fclose(out);
}

static void get_ack_time(ac_task_part_t *tp) {
  time_t *ack = &(tp->task->state_linkage[tp->partition].ack_time);
  if (*ack == -1) {
    char *filename =
        ac_pool_strdupf(tp->pool, "%s/%s_%lu", tp->task->scheduler->ack_dir,
                        tp->task->task_name, tp->partition);
    *ack = ac_io_modified(filename);
  }
  tp->ack_time = *ack;
}

static ac_task_part_t *task_part_complete(ac_task_part_t *tp, time_t when) {
  if (when > tp->ack_time)
    write_ack(tp);
  // ac_task_part_t *next = NULL;
  ac_schedule_t *scheduler = tp->task->scheduler;
  pthread_mutex_lock(&(scheduler->mutex));
  size_t num_available = scheduler->num_available;
  mark_task_complete(tp->__link, tp->partition, when);
  // if (tp->next_task && take_available(tp->next_task, tp->partition))
  //  next = take_task(tp->next_task, tp->partition);
  if (!num_available && scheduler->num_available)
    pthread_cond_broadcast(&scheduler->cond);
  pthread_mutex_unlock(&(scheduler->mutex));
  return NULL;
}

static ac_task_part_t *get_next_task(ac_pool_t *pool, ac_schedule_thread_t *t) {
  ac_task_part_t *tp = NULL;
  ac_schedule_t *scheduler = t->scheduler;
  // printf("Attempting to get task for %lu\n", t->thread_id);
  pthread_mutex_lock(&(scheduler->mutex));
  scheduler->num_running--;
  while (!scheduler->done && !scheduler->num_available &&
         scheduler->num_tasks_to_run)
    pthread_cond_wait(&scheduler->cond, &scheduler->mutex);

  if (scheduler->num_available) {
    for (size_t p = 0; p < scheduler->num_partitions; p++) {
      size_t px = (p + t->partition) % scheduler->num_partitions;
      tp = take_task(scheduler, pool, NULL, px);
      if (tp) {
        scheduler->num_running++;
        tp->running = scheduler->num_running + scheduler->num_available;
        if (tp->running > scheduler->cpus)
          tp->running = scheduler->cpus;
        break;
      }
    }
  }
  pthread_mutex_unlock(&(scheduler->mutex));
  if (tp)
    tp->thread_id = t->thread_id;
  // if (!tp) {
  //  printf("failed to get task for %lu\n", t->thread_id);
  // }
  return tp;
}

static void setup_task_part(ac_task_part_t *tp) {}
static bool run_task_part(ac_task_part_t *tp) {
  if (tp->task->runner)
    return tp->task->runner(tp);
  return true;
}

time_t get_ack_time_for_task_and_partition(ac_task_t *task, size_t partition) {
  if (partition >= task->num_partitions)
    return 0;
  return task->state_linkage[partition].ack_time;
}

static bool task_needs_to_run(ac_task_part_t *tp) {
  if (tp->ack_time == 0)
    return true;

  if (tp->task->run_everytime)
    return true;

  ac_task_t *task = tp->task;
  ac_task_link_t *link = task->reverse_dependencies;
  while (link) {
    for (size_t i = 0; i < link->task->num_partitions; i++) {
      time_t ack_time = get_ack_time_for_task_and_partition(link->task, i);
      if (ack_time > tp->ack_time)
        return true;
    }
    link = link->next;
  }
  link = task->reverse_partial_dependencies;
  while (link) {
    time_t ack_time =
        get_ack_time_for_task_and_partition(link->task, tp->partition);
    if (ack_time > tp->ack_time)
      return true;
    link = link->next;
  }

  if (tp->task->inputs) {
    ac_task_input_t *n = tp->task->inputs;
    while (n) {
      if (n->check(tp, n, tp->ack_time))
        return true;
      n = n->next;
    }
  }
  return false;
}

static void destroy_task_part(ac_task_part_t *tp) {}

static void mark_as_done(ac_schedule_t *scheduler) {
  pthread_mutex_unlock(&(scheduler->mutex));
  if (!scheduler->done) {
    scheduler->done = true;
    pthread_cond_broadcast(&scheduler->cond);
  }
  pthread_mutex_unlock(&(scheduler->mutex));
}

static char *_ac_task_output_base(ac_task_part_t *tp, ac_task_output_t *outp,
                                  const char *suffix) {
  // base_<part><suffix>
  ac_buffer_setf(tp->bh, "%s/%s_%lu/", tp->task->scheduler->task_dir,
                 tp->task->task_name, tp->partition);
  const char *base = outp->name;
  if (ac_io_extension(base, ".lz4")) {
    ac_buffer_append(tp->bh, base, strlen(base) - 4);
    if (suffix)
      ac_buffer_appends(tp->bh, suffix);
    ac_buffer_appendf(tp->bh, "_%lu.lz4", tp->partition);
    return ac_buffer_data(tp->bh);
  } else if (ac_io_extension(base, ".gz")) {
    ac_buffer_append(tp->bh, base, strlen(base) - 3);
    if (suffix)
      ac_buffer_appends(tp->bh, suffix);
    ac_buffer_appendf(tp->bh, "_%lu.gz", tp->partition);
    return ac_buffer_data(tp->bh);
  } else {
    ac_buffer_appends(tp->bh, base);
    if (suffix)
      ac_buffer_appends(tp->bh, suffix);
    ac_buffer_appendf(tp->bh, "_%lu", tp->partition);
    return ac_buffer_data(tp->bh);
  }
}

char *ac_task_output_base(ac_task_part_t *tp, ac_task_output_t *outp) {
  return _ac_task_output_base(tp, outp, NULL);
}

char *ac_task_output_base2(ac_task_part_t *tp, ac_task_output_t *outp,
                           const char *suffix) {
  return _ac_task_output_base(tp, outp, suffix);
}

char *ac_task_input_name(ac_task_part_t *tp, ac_task_input_t *inp,
                         size_t partition) {
  const char *base = inp->name;
  ac_buffer_setf(tp->bh, "%s/%s_%lu/", tp->task->scheduler->task_dir,
                 inp->src->task->task_name, partition);
  if (ac_io_extension(base, ".lz4")) {
    ac_buffer_append(tp->bh, base, strlen(base) - 4);
    ac_buffer_appendf(tp->bh, "_%lu_%lu.lz4", partition, tp->partition);
    return ac_buffer_data(tp->bh);
  } else if (ac_io_extension(base, ".gz")) {
    ac_buffer_append(tp->bh, base, strlen(base) - 3);
    ac_buffer_appendf(tp->bh, "_%lu_%lu.gz", partition, tp->partition);
    return ac_buffer_data(tp->bh);
  } else {
    ac_buffer_appends(tp->bh, base);
    ac_buffer_appendf(tp->bh, "_%lu_%lu", partition, tp->partition);
    return ac_buffer_data(tp->bh);
  }
}

size_t ac_task_part_ram(ac_task_part_t *tp, double pct) {
  double total_ram = tp->task->scheduler->ram;
  double running = tp->running;
  running = running / pct;
  size_t res = (size_t)round((total_ram * pct) / running);
  return res * 1024;
}

void *schedule_thread(void *arg) {
  ac_schedule_thread_t *t = (ac_schedule_thread_t *)arg;
  ac_pool_t *pool = ac_pool_init(65536);
  while (true) {
    ac_pool_clear(pool);
    ac_task_part_t *tp = get_next_task(pool, t);
    if (!tp)
      break;

    tp->bh = ac_buffer_pool_init(pool, 200);
    get_ack_time(tp);

    if (task_needs_to_run(tp)) {
      setup_task_part(tp);
      if (!run_task_part(tp)) {
        destroy_task_part(tp);
        break;
      }
      if (t->scheduler->on_complete) {
        if (!t->scheduler->on_complete(tp)) {
          destroy_task_part(tp);
          break;
        }
      }
      task_part_complete(tp, time(NULL));
      destroy_task_part(tp);
    } else
      task_part_complete(tp, tp->ack_time);
  }
  ac_pool_destroy(pool);
  mark_as_done(t->scheduler);
  return NULL;
}

void ac_task_runner(ac_task_t *task, ac_task_part_f runner) {
  task->runner = runner;
}

void ac_schedule_run(ac_schedule_t *h, ac_task_part_f on_complete) {
  schedule_setup(h);
  h->num_running = h->cpus;
  h->on_complete = on_complete;
  h->done = false;
  for (size_t i = 0; i < h->cpus; i++)
    pthread_create(&(h->threads[i].thread), NULL, schedule_thread,
                   h->threads + i);
  for (size_t i = 0; i < h->cpus; i++)
    pthread_join(h->threads[i].thread, NULL);
  return;
}

ac_task_t *ac_schedule_task(ac_schedule_t *h, const char *task_name,
                            bool partitioned, ac_task_f setup) {
  ac_task_t *node = _task_find(task_name, h->task_root);
  if (!node) {
    size_t num_partitions = 1;
    if (partitioned)
      num_partitions = h->num_partitions;

    node = (ac_task_t *)ac_pool_calloc(
        h->pool, sizeof(ac_task_t) + strlen(task_name) + 1 +
                     (num_partitions * sizeof(ac_task_state_link_t)));
    node->scheduler = h;
    node->do_nothing = false;
    node->run_everytime = false;
    node->setup = setup;
    node->state_linkage = (ac_task_state_link_t *)(node + 1);
    node->num_partitions = num_partitions;
    node->task_name = (char *)(node->state_linkage + num_partitions);
    strcpy(node->task_name, task_name);
    _task_insert(node, &(h->task_root));
    for (size_t i = 0; i < num_partitions; i++) {
      node->state_linkage[i].task = node;
      node->state_linkage[i].waiting_on_others = false;
      node->state_linkage[i].completed = 0;
      node->state_linkage[i].ack_time = -1;
      link_state(h, node->state_linkage + i, i);
    }
    node->next = NULL;
    if (!h->head)
      h->head = h->tail = node;
    else {
      h->tail->next = node;
      h->tail = node;
    }
  }
  return node;
}

void ac_task_run_everytime(ac_task_t *task) { task->run_everytime = true; }
void ac_task_do_nothing(ac_task_t *task) { task->do_nothing = true; }

static bool is_task_complete(ac_task_t *task) {
  for (size_t i = 0; i < task->num_partitions; i++) {
    if (task->state_linkage[i].waiting_on_others ||
        !task->state_linkage[i].completed)
      return false;
  }
  return true;
}

static bool is_task_partition_complete(ac_task_t *task, size_t partition) {
  if (partition >= task->num_partitions)
    return is_task_partition_complete(task, 0);

  if (!task->state_linkage[partition].waiting_on_others &&
      task->state_linkage[partition].completed)
    return true;
  return false;
}

static bool is_dependencies_complete(ac_task_t *task, size_t partition) {
  // dependencies
  ac_task_link_t *n = task->dependencies;
  while (n) {
    if (!is_task_complete(n->task))
      return false;
    n = n->next;
  }

  // partial_dependencies
  n = task->partial_dependencies;
  while (n) {
    if (!is_task_partition_complete(n->task, partition))
      return false;
    n = n->next;
  }
  return true;
}

static void add_task_link(ac_task_t *task, ac_task_link_t **link,
                          ac_task_t *to_add) {
  ac_task_link_t *n = *link;
  while (n) {
    if (n->task == to_add)
      return;
    n = n->next;
  }

  ac_schedule_t *scheduler = task->scheduler;
  n = (ac_task_link_t *)ac_pool_calloc(scheduler->pool, sizeof(ac_task_link_t));
  n->task = to_add;
  n->next = *link;
  *link = n;
}

static bool _ac_task_dependency(ac_task_t *task, const char *dependency,
                                bool partial) {
  ac_schedule_t *scheduler = task->scheduler;
  if (strchr(dependency, '|')) {
    ac_pool_clear(scheduler->tmp_pool);
    char **dep = ac_pool_split2(scheduler->tmp_pool, NULL, '|', dependency);
    for (size_t i = 0; dep[i] != NULL; i++) {
      if (!_ac_task_dependency(task, dep[i], partial))
        return false;
    }
    return true;
  }
  ac_task_t *d = find_task(scheduler, dependency);
  if (!d) {
    printf("%s not found in scheduler\n", dependency);
    printf("   - the task must first be added to scheduler before being "
           "used as a dependency\n");
  }
  if (!task || !d)
    return false;

  if (partial) {
    add_task_link(task, &task->partial_dependencies, d);
    add_task_link(d, &d->reverse_partial_dependencies, task);
  } else {
    add_task_link(task, &task->dependencies, d);
    add_task_link(d, &d->reverse_dependencies, task);
  }

  if (!task->state_linkage[0].waiting_on_others) {
    for (size_t i = 0; i < task->num_partitions; i++) {
      if (is_task_partition_complete(d, i))
        continue;

      ac_task_state_link_t *state_link = task->state_linkage + i;
      if (!state_link->waiting_on_others) {
        unlink_state(scheduler, state_link, i);
        state_link->waiting_on_others = true;
        link_state(scheduler, state_link, i);
      }
    }
  }
  return true;
}

bool ac_task_dependency(ac_task_t *task, const char *dependency) {
  return _ac_task_dependency(task, dependency, false);
}

bool ac_task_partial_dependency(ac_task_t *task, const char *dependency) {
  return _ac_task_dependency(task, dependency, true);
}

static void check_task(ac_schedule_t *scheduler,
                       ac_task_state_link_t *state_link, size_t partition,
                       time_t when) {
  if (state_link->waiting_on_others &&
      is_dependencies_complete(state_link->task, partition)) {
    unlink_state(scheduler, state_link, partition);
    state_link->waiting_on_others = false;
    if (state_link->task->do_nothing)
      mark_task_complete(state_link, partition, when);
    else
      link_state(scheduler, state_link, partition);
  }
}

static void mark_task_complete(ac_task_state_link_t *state_link,
                               size_t partition, time_t when) {
  ac_task_t *task = state_link->task;
  ac_schedule_t *scheduler = task->scheduler;
  state_link->waiting_on_others = false;
  state_link->completed = when;
  link_state(scheduler, state_link, partition);

  bool partially_complete = false;
  for (size_t i = 0; i < task->num_partitions; i++) {
    if (!task->state_linkage[i].completed) {
      partially_complete = true;
      break;
    }
  }

  if (!partially_complete) {
    ac_task_link_t *link = task->reverse_dependencies;
    while (link) {
      for (size_t i = 0; i < link->task->num_partitions; i++) {
        ac_task_state_link_t *state_link = link->task->state_linkage + i;
        check_task(scheduler, state_link, i, when);
      }
      link = link->next;
    }
    link = task->reverse_partial_dependencies;
    while (link) {
      for (size_t i = 0; i < link->task->num_partitions; i++) {
        ac_task_state_link_t *state_link = link->task->state_linkage + i;
        check_task(scheduler, state_link, i, when);
      }
      link = link->next;
    }
  } else {
    ac_task_link_t *link = task->reverse_partial_dependencies;
    while (link) {
      size_t i = partition;
      if (i < link->task->num_partitions) {
        ac_task_state_link_t *state_link = link->task->state_linkage + i;
        check_task(scheduler, state_link, i, when);
      }
      link = link->next;
    }
  }
}

const char *ac_task_name(ac_task_t *task) { return task->task_name; }
