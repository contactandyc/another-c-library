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

struct ac_schedule_thread_s {
  pthread_t thread;
  ac_schedule_t *scheduler;
  ac_pool_t *pool;
  ac_buffer_t *bh;
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

  ac_worker_f on_complete;
  bool done;

  size_t num_partitions;
  ac_task_state_t *state;

  ac_task_t *head;
  ac_task_t *tail;

  size_t ram;
  size_t cpus;
  size_t disk_space;
};

struct ac_task_input_link_s;
typedef struct ac_task_input_link_s ac_task_input_link_t;

struct ac_task_input_link_s {
  ac_worker_input_t *input;
  ac_task_input_link_t *next;
};

struct ac_transform_s;
typedef struct ac_transform_s ac_transform_t;

struct ac_transform_s {
  ac_worker_input_t *input;
  ac_worker_output_t **outputs;
  size_t num_outputs;
  ac_runner_f runner;
  ac_group_runner_f group_runner;
  ac_io_compare_f group_compare;
  ac_worker_data_f create_group_compare_arg;
  ac_destroy_worker_data_f destroy_group_compare_arg;
  ac_worker_data_f create_data;
  ac_destroy_worker_data_f destroy_data;
  ac_transform_t *next;
};

struct ac_task_s {
  ac_map_t node;

  char *task_name;
  size_t num_partitions;

  bool do_nothing;
  bool run_everytime;

  ac_schedule_t *scheduler;

  ac_task_f setup;
  ac_worker_f runner;

  ac_transform_t *transforms;

  ac_task_input_link_t *current_input;

  ac_worker_input_t *inputs;
  ac_worker_output_t *outputs;
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

ac_worker_input_t *ac_task_find_input(ac_task_t *task, const char *name) {
  ac_worker_input_t *n = task->inputs;
  while (n) {
    if (!strcmp(n->name, name))
      return n;
    n = n->next;
  }
  return NULL;
}

ac_worker_output_t *ac_task_find_output(ac_task_t *task, const char *name) {
  ac_worker_output_t *n = task->outputs;
  while (n) {
    if (!strcmp(n->name, name))
      return n;
    n = n->next;
  }
  return NULL;
}

static ac_transform_t *_ac_task_transform(ac_task_t *task, const char *inp,
                                          const char *outp) {
  ac_pool_t *pool = task->scheduler->pool;
  ac_transform_t *t = (ac_transform_t *)ac_pool_calloc(pool, sizeof(*t));
  t->input = ac_task_find_input(task, inp);
  if (!t->input)
    abort();
  size_t num_outputs = 0;
  char **outputs = ac_pool_split2(pool, &num_outputs, '|', outp);
  t->outputs = (ac_worker_output_t **)ac_pool_calloc(
      pool, sizeof(ac_worker_output_t *) * (num_outputs + 1));
  for (size_t i = 0; i < num_outputs; i++) {
    t->outputs[i] = ac_task_find_output(task, outputs[i]);
    if (!t->outputs[i])
      abort();
  }
  t->num_outputs = num_outputs;

  if (!task->transforms)
    task->transforms = t;
  else {
    ac_transform_t *n = task->transforms;
    while (n->next)
      n = n->next;
    n->next = t;
  }
  return t;
}

void ac_task_transform(ac_task_t *task, const char *inp, const char *outp,
                       ac_runner_f runner) {
  ac_transform_t *t = _ac_task_transform(task, inp, outp);
  t->runner = runner;
}

void ac_task_group_transform(ac_task_t *task, const char *inp, const char *outp,
                             ac_group_runner_f runner,
                             ac_io_compare_f compare) {
  ac_transform_t *t = _ac_task_transform(task, inp, outp);
  t->group_runner = runner;
  t->group_compare = compare;
}

void ac_task_group_compare_arg(ac_task_t *task, ac_worker_data_f create,
                               ac_destroy_worker_data_f destroy) {
  ac_transform_t *n = task->transforms;
  while (n->next)
    n = n->next;
  n->create_group_compare_arg = create;
  n->destroy_group_compare_arg = destroy;
}

void ac_task_transform_data(ac_task_t *task, ac_worker_data_f create,
                            ac_destroy_worker_data_f destroy) {
  ac_transform_t *n = task->transforms;
  while (n->next)
    n = n->next;
  n->create_data = create;
  n->destroy_data = destroy;
}

ac_worker_output_t *ac_worker_output(ac_worker_t *w, size_t pos) {
  ac_worker_output_t *r = w->outputs;
  while (r && pos) {
    pos--;
    r = r->next;
  }
  if (pos)
    return NULL;
  return r;
}

ac_out_t *ac_worker_out(ac_worker_t *w, size_t n) {
  ac_worker_output_t *o = ac_worker_output(w, n);
  if (!o)
    return NULL;
  size_t flags = o->flags;
  char *base_name = ac_worker_output_base(w, o);
  ac_out_options_buffer_size(&(o->options), ac_worker_ram(w, o->ram_pct));
  if ((flags & AC_OUTPUT_SPLIT) && o->ext_options.partition) {
    if (o->destinations)
      ac_out_ext_options_num_partitions(&(o->ext_options),
                                        o->destinations->task->num_partitions);
    else
      ac_out_ext_options_num_partitions(&(o->ext_options),
                                        o->task->scheduler->num_partitions);
    return ac_out_ext_init(base_name, &(o->options), &(o->ext_options));
  } else {
    o->ext_options.partition = NULL;
    return ac_out_ext_init(base_name, &(o->options), &(o->ext_options));
  }
}

ac_worker_input_t *ac_worker_input(ac_worker_t *w, size_t pos) {
  ac_worker_input_t *r = w->inputs;
  while (r && pos) {
    pos--;
    r = r->next;
  }
  if (pos)
    return NULL;
  return r;
}

ac_in_t *ac_worker_in(ac_worker_t *w, size_t n) {
  ac_worker_input_t *inp = ac_worker_input(w, n);
  if (!inp || !inp->num_files)
    return NULL;

  ac_in_t *in = NULL;
  if (inp->compare && inp->num_files > 1) {
    ac_in_options_buffer_size(&(inp->options),
                              ac_worker_ram(w, inp->ram_pct / inp->num_files));
    in = ac_in_ext_init(inp->compare, inp->compare_arg, &(inp->options));
    if (inp->reducer)
      ac_in_ext_reducer(in, inp->reducer, inp->reducer_arg);
    for (size_t i = 0; i < inp->num_files; i++)
      ac_in_ext_add(in, ac_in_init(inp->files[i].filename, &(inp->options)),
                    inp->files[i].tag);
  } else {
    ac_in_options_buffer_size(&(inp->options), ac_worker_ram(w, inp->ram_pct));
    if (inp->num_files > 1)
      in = ac_in_init_from_list(inp->files, inp->num_files, &(inp->options));
    else {
      ac_in_options_t opts = inp->options;
      if (opts.buffer_size > inp->files[0].size)
        opts.buffer_size = inp->files[0].size;
      opts.tag = inp->files[0].tag;
      in = ac_in_init(inp->files[0].filename, &opts);
    }
  }
  if (inp->limit)
    ac_in_limit(in, inp->limit);
  return in;
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

static ac_worker_t *take_worker(ac_schedule_t *scheduler, ac_pool_t *pool,
                                ac_task_t *task, size_t partition) {
  ac_task_state_link_t *avail = available_tasks(scheduler, partition);
  while (avail) {
    if (!task || avail->task == task) {
      unlink_state(scheduler, avail, partition);
      ac_worker_t *w = (ac_worker_t *)ac_pool_calloc(pool, sizeof(ac_worker_t));
      w->task = avail->task;
      w->partition = partition;
      w->num_partitions = w->task->num_partitions;
      w->ack_time = -1;
      w->__link = avail;
      return w;
    }
    avail = avail->next;
  }
  return NULL;
}

static ac_worker_input_t *_ac_task_input(ac_task_t *task, const char *name,
                                         ac_worker_output_t *src, double pct,
                                         ac_worker_file_info_f file_info) {
  ac_schedule_t *scheduler = task->scheduler;
  ac_pool_t *pool = scheduler->pool;

  ac_worker_input_t *ti =
      (ac_worker_input_t *)ac_pool_calloc(pool, sizeof(*ti));
  ac_in_options_init(&(ti->options));
  ti->name = ac_pool_strdup(pool, name);
  ti->ram_pct = pct;
  ti->task = task;
  ti->src = src;
  ti->file_info = file_info;
  if (task->inputs) {
    ac_worker_input_t *n = task->inputs;
    while (n->next)
      n = n->next;
    ti->id = n->id + 1;
    n->next = ti;
  } else {
    ti->id = 0;
    task->inputs = ti;
  }
  return ti;
}

ac_io_file_info_t *file_info_split(ac_worker_t *w, size_t *num_files,
                                   ac_worker_input_t *inp) {
  size_t num_partitions = inp->src->task->num_partitions;
  ac_io_file_info_t *res = (ac_io_file_info_t *)ac_pool_calloc(
      w->worker_pool, sizeof(*res) * num_partitions);
  for (size_t i = 0; i < num_partitions; i++) {
    res[i].filename = ac_worker_input_name(w, inp, i);
    ac_io_file_info(res + i);
  }
  *num_files = num_partitions;
  return res;
}

ac_io_file_info_t *file_info_first(ac_worker_t *w, size_t *num_files,
                                   ac_worker_input_t *inp) {
  // check if input files are newer than ack file
  ac_io_file_info_t *res =
      (ac_io_file_info_t *)ac_pool_calloc(w->worker_pool, sizeof(*res));
  res->filename = ac_worker_input_name(w, inp, 0);
  *num_files = 1;
  ac_io_file_info(res);
  return res;
}

ac_io_file_info_t *file_info_name(ac_worker_t *w, size_t *num_files,
                                  ac_worker_input_t *inp) {
  // check if input files are newer than ack file
  ac_io_file_info_t *res =
      (ac_io_file_info_t *)ac_pool_calloc(w->worker_pool, sizeof(*res));
  res->filename = inp->name;
  *num_files = 1;
  ac_io_file_info(res);
  return res;
}

ac_io_file_info_t *file_info_partition(ac_worker_t *w, size_t *num_files,
                                       ac_worker_input_t *inp) {
  // check if input files are newer than ack file
  ac_io_file_info_t *res =
      (ac_io_file_info_t *)ac_pool_calloc(w->worker_pool, sizeof(*res));
  res->filename = ac_worker_input_name(w, inp, w->partition);
  *num_files = 1;
  ac_io_file_info(res);
  return res;
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

  ac_worker_output_t *to =
      (ac_worker_output_t *)ac_pool_calloc(pool, sizeof(*to));
  ac_out_options_init(&(to->options));
  ac_out_ext_options_init(&(to->ext_options));

  to->name = ac_pool_strdup(pool, name);
  to->task = task;
  to->ram_pct = out_ram_pct;
  to->destinations = get_task_list(task, name, scheduler, destinations);
  to->flags = flags;
  to->num_partitions = task->num_partitions;

  if (task->outputs) {
    ac_worker_output_t *n = task->outputs;
    while (n->next)
      n = n->next;
    to->id = n->id + 1;
    n->next = to;
  } else {
    to->id = 0;
    task->outputs = to;
  }

  task->current_input = NULL;

  ac_task_link_t *n = to->destinations;
  while (n) {
    ac_worker_file_info_f file_info = NULL;
    if (flags & AC_OUTPUT_SPLIT)
      file_info = file_info_split;
    else if (flags & AC_OUTPUT_USE_FIRST)
      file_info = file_info_first;
    else
      file_info = file_info_partition;
    ac_worker_input_t *ti =
        _ac_task_input(n->task, name, to, in_ram_pct, file_info);
    if (ti) {
      ac_task_input_link_t *inp_link =
          (ac_task_input_link_t *)ac_pool_alloc(pool, sizeof(*inp_link));
      inp_link->input = ti;
      inp_link->next = task->current_input;
      task->current_input = inp_link;
    }
    n = n->next;
  }
}

void ac_task_input_files(ac_task_t *task, const char *name, double pct,
                         ac_worker_file_info_f file_info) {
  ac_schedule_t *scheduler = task->scheduler;
  ac_pool_t *pool = scheduler->pool;
  if (!file_info)
    file_info = file_info_name;

  task->current_input = NULL;
  ac_worker_input_t *ti = _ac_task_input(task, name, NULL, pct, file_info);
  if (ti) {
    ac_task_input_link_t *inp_link =
        (ac_task_input_link_t *)ac_pool_alloc(pool, sizeof(*inp_link));
    inp_link->input = ti;
    inp_link->next = task->current_input;
    task->current_input = inp_link;
  }
}

/* These ac_task_output_... methods must be called after ac_task_output and
   will apply to the previous ac_task_output call. */
void ac_task_output_partition(ac_task_t *task, ac_io_partition_f part,
                              void *arg) {
  if (!task->outputs)
    return;

  ac_out_ext_options_partition(&(task->outputs->ext_options), part, arg);
}

void ac_task_output_compare(ac_task_t *task, ac_io_compare_f compare,
                            void *arg) {
  if (!task->outputs)
    return;

  ac_out_ext_options_compare(&(task->outputs->ext_options), compare, arg);
  ac_task_input_compare(task, compare, arg);
}

void ac_task_output_intermediate_compare(ac_task_t *task,
                                         ac_io_compare_f compare, void *arg) {
  if (!task->outputs)
    return;

  ac_out_ext_options_intermediate_compare(&(task->outputs->ext_options),
                                          compare, arg);
}

void ac_task_output_keep_first(ac_task_t *task) {
  if (!task->outputs)
    return;

  ac_out_ext_options_reducer(&(task->outputs->ext_options), ac_io_keep_first,
                             NULL);
  ac_task_input_keep_first(task);
}

void ac_task_output_reducer(ac_task_t *task, ac_io_reducer_f reducer,
                            void *arg) {
  if (!task->outputs)
    return;

  ac_out_ext_options_reducer(&(task->outputs->ext_options), reducer, arg);
  ac_task_input_reducer(task, reducer, arg);
}

void ac_task_output_intermediate_reducer(ac_task_t *task,
                                         ac_io_reducer_f reducer, void *arg) {
  if (!task->outputs)
    return;

  ac_out_ext_options_intermediate_reducer(&(task->outputs->ext_options),
                                          reducer, arg);
}

void ac_task_output_group_size(ac_task_t *task, size_t num_per_group,
                               size_t start) {
  if (!task->outputs)
    return;

  ac_out_ext_options_intermediate_group_size(&(task->outputs->ext_options),
                                             num_per_group);
}

void ac_task_output_use_extra_thread(ac_task_t *task) {
  if (!task->outputs)
    return;

  ac_out_ext_options_use_extra_thread(&(task->outputs->ext_options));
}

void ac_task_output_dont_compress_tmp(ac_task_t *task) {
  if (!task->outputs)
    return;

  ac_out_ext_options_dont_compress_tmp(&(task->outputs->ext_options));
}

void ac_task_output_sort_before_partitioning(ac_task_t *task) {
  if (!task->outputs)
    return;

  ac_out_ext_options_sort_before_partitioning(&(task->outputs->ext_options));
}

void ac_task_output_sort_while_partitioning(ac_task_t *task) {
  if (!task->outputs)
    return;

  ac_out_ext_options_sort_while_partitioning(&(task->outputs->ext_options));
}

void ac_task_output_num_sort_threads(ac_task_t *task, size_t num_sort_threads) {
  if (!task->outputs)
    return;

  ac_out_ext_options_num_sort_threads(&(task->outputs->ext_options),
                                      num_sort_threads);
}

void ac_task_output_format(ac_task_t *task, ac_io_format_t format) {
  if (!task->outputs)
    return;

  ac_out_options_format(&(task->outputs->options), format);
  ac_task_input_format(task, format);
}

void ac_task_output_safe_mode(ac_task_t *task) {
  if (!task->outputs)
    return;

  ac_out_options_safe_mode(&(task->outputs->options));
}

void ac_task_output_write_ack_file(ac_task_t *task) {
  if (!task->outputs)
    return;

  ac_out_options_write_ack_file(&(task->outputs->options));
}

void ac_task_output_gz(ac_task_t *task, int level) {
  if (!task->outputs)
    return;

  ac_out_options_gz(&(task->outputs->options), level);
}

void ac_task_output_lz4(ac_task_t *task, int level, ac_lz4_block_size_t size,
                        bool block_checksum, bool content_checksum) {
  if (!task->outputs)
    return;

  ac_out_options_lz4(&(task->outputs->options), level, size, block_checksum,
                     content_checksum);
}

/* The ac_task_input... methods apply to the previous ac_task_input_files or
   ac_task_output call.  If the previous ac_task_output call doesn't specify
   one or more destinations, the calls are silently ignored. */
void ac_task_input_format(ac_task_t *task, ac_io_format_t format) {
  ac_task_input_link_t *inp = task->current_input;
  while (inp) {
    ac_in_options_format(&(inp->input->options), format);
    inp = inp->next;
  }
}

void ac_task_input_compare(ac_task_t *task, ac_io_compare_f compare,
                           void *arg) {
  ac_task_input_link_t *inp = task->current_input;
  while (inp) {
    inp->input->compare = compare;
    inp->input->compare_arg = arg;
    inp = inp->next;
  }
}

void ac_task_input_keep_first(ac_task_t *task) {
  ac_task_input_reducer(task, ac_io_keep_first, NULL);
}

void ac_task_input_reducer(ac_task_t *task, ac_io_reducer_f reducer,
                           void *arg) {
  ac_task_input_link_t *inp = task->current_input;
  while (inp) {
    inp->input->reducer = reducer;
    inp->input->reducer_arg = arg;
    inp = inp->next;
  }
}

void ac_task_input_compressed_buffer_size(ac_task_t *task, size_t buffer_size) {
  ac_task_input_link_t *inp = task->current_input;
  while (inp) {
    ac_in_options_compressed_buffer_size(&(inp->input->options), buffer_size);
    inp = inp->next;
  }
}

void ac_task_input_limit(ac_task_t *task, size_t limit) {
  ac_task_input_link_t *inp = task->current_input;
  while (inp) {
    inp->input->limit = limit;
    inp = inp->next;
  }
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
          ac_worker_t *w = take_worker(h, h->tmp_pool, n, i);
          if (w)
            mark_task_complete(w->__link, i, current_time);
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

static void write_ack(ac_worker_t *w) {
  char *filename = ac_pool_strdupf(w->schedule_thread->pool, "%s/%s_%lu",
                                   w->task->scheduler->ack_dir,
                                   w->task->task_name, w->partition);
  // printf("%s\n", filename);
  FILE *out = fopen(filename, "wb");
  fclose(out);
}

static void get_ack_time(ac_worker_t *w) {
  time_t *ack = &(w->task->state_linkage[w->partition].ack_time);
  if (*ack == -1) {
    char *filename = ac_pool_strdupf(w->schedule_thread->pool, "%s/%s_%lu",
                                     w->task->scheduler->ack_dir,
                                     w->task->task_name, w->partition);
    *ack = ac_io_modified(filename);
  }
  w->ack_time = *ack;
}

static ac_worker_t *worker_complete(ac_worker_t *w, time_t when) {
  if (when > w->ack_time)
    write_ack(w);
  // ac_worker_t *next = NULL;
  ac_schedule_t *scheduler = w->task->scheduler;
  pthread_mutex_lock(&(scheduler->mutex));
  size_t num_available = scheduler->num_available;
  mark_task_complete(w->__link, w->partition, when);
  // if (w->next_task && take_available(w->next_task, w->partition))
  //  next = take_worker(w->next_task, w->partition);
  if (!num_available && scheduler->num_available)
    pthread_cond_broadcast(&scheduler->cond);
  pthread_mutex_unlock(&(scheduler->mutex));
  return NULL;
}

static ac_worker_t *get_next_worker(ac_schedule_thread_t *t) {
  ac_pool_t *pool = t->pool;
  ac_worker_t *w = NULL;
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
      w = take_worker(scheduler, pool, NULL, px);
      if (w) {
        scheduler->num_running++;
        w->running = scheduler->num_running + scheduler->num_available;
        if (w->running > scheduler->cpus)
          w->running = scheduler->cpus;
        break;
      }
    }
  }
  pthread_mutex_unlock(&(scheduler->mutex));
  if (w) {
    w->thread_id = t->thread_id;
    w->schedule_thread = t;
  }
  // if (!w) {
  //  printf("failed to get task for %lu\n", t->thread_id);
  // }
  return w;
}

static void setup_worker(ac_worker_t *w) {}
static bool run_worker(ac_worker_t *w) {
  bool r = true;
  w->timer = ac_timer_init(1);
  ac_timer_start(w->timer);
  if (w->task->runner)
    r = w->task->runner(w);
  ac_timer_stop(w->timer);
  return true;
}

time_t get_ack_time_for_task_and_partition(ac_task_t *task, size_t partition) {
  if (partition >= task->num_partitions)
    return 0;
  return task->state_linkage[partition].ack_time;
}

static bool worker_needs_to_run(ac_worker_t *w) {
  if (w->ack_time == 0)
    return true;

  if (w->task->run_everytime)
    return true;

  ac_task_t *task = w->task;
  ac_task_link_t *link = task->reverse_dependencies;
  while (link) {
    for (size_t i = 0; i < link->task->num_partitions; i++) {
      time_t ack_time = get_ack_time_for_task_and_partition(link->task, i);
      if (ack_time > w->ack_time)
        return true;
    }
    link = link->next;
  }
  link = task->reverse_partial_dependencies;
  while (link) {
    time_t ack_time =
        get_ack_time_for_task_and_partition(link->task, w->partition);
    if (ack_time > w->ack_time)
      return true;
    link = link->next;
  }

  if (w->inputs) {
    ac_worker_input_t *n = w->inputs;
    while (n) {
      for (size_t i = 0; i < n->num_files; i++) {
        if (n->files[i].last_modified > w->ack_time)
          return true;
      }
      n = n->next;
    }
  }
  return false;
}

static void destroy_worker(ac_worker_t *w) {
  if (w->timer)
    ac_timer_destroy(w->timer);
}

static void mark_as_done(ac_schedule_t *scheduler) {
  pthread_mutex_unlock(&(scheduler->mutex));
  if (!scheduler->done) {
    scheduler->done = true;
    pthread_cond_broadcast(&scheduler->cond);
  }
  pthread_mutex_unlock(&(scheduler->mutex));
}

static char *_ac_worker_output_base(ac_worker_t *w, ac_worker_output_t *outp,
                                    const char *suffix) {
  // base_<part><suffix>
  ac_buffer_t *bh = w->schedule_thread->bh;
  ac_buffer_setf(bh, "%s/%s_%lu/", w->task->scheduler->task_dir,
                 w->task->task_name, w->partition);
  const char *base = outp->name;
  if (ac_io_extension(base, ".lz4")) {
    ac_buffer_append(bh, base, strlen(base) - 4);
    if (suffix)
      ac_buffer_appends(bh, suffix);
    ac_buffer_appendf(bh, "_%lu.lz4", w->partition);
  } else if (ac_io_extension(base, ".gz")) {
    ac_buffer_append(bh, base, strlen(base) - 3);
    if (suffix)
      ac_buffer_appends(bh, suffix);
    ac_buffer_appendf(bh, "_%lu.gz", w->partition);
  } else {
    ac_buffer_appends(bh, base);
    if (suffix)
      ac_buffer_appends(bh, suffix);
    ac_buffer_appendf(bh, "_%lu", w->partition);
  }
  return ac_pool_strdup(w->worker_pool, ac_buffer_data(bh));
}

char *ac_worker_output_base(ac_worker_t *w, ac_worker_output_t *outp) {
  return _ac_worker_output_base(w, outp, NULL);
}

char *ac_worker_output_base2(ac_worker_t *w, ac_worker_output_t *outp,
                             const char *suffix) {
  return _ac_worker_output_base(w, outp, suffix);
}

char *ac_worker_input_name(ac_worker_t *w, ac_worker_input_t *inp,
                           size_t partition) {
  const char *base = inp->name;
  ac_buffer_t *bh = w->schedule_thread->bh;
  ac_buffer_setf(bh, "%s/%s_%lu/", w->task->scheduler->task_dir,
                 inp->src->task->task_name, partition);
  if (ac_io_extension(base, ".lz4")) {
    ac_buffer_append(bh, base, strlen(base) - 4);
    ac_buffer_appendf(bh, "_%lu_%lu.lz4", partition, w->partition);
  } else if (ac_io_extension(base, ".gz")) {
    ac_buffer_append(bh, base, strlen(base) - 3);
    ac_buffer_appendf(bh, "_%lu_%lu.gz", partition, w->partition);
  } else {
    ac_buffer_appends(bh, base);
    ac_buffer_appendf(bh, "_%lu_%lu", partition, w->partition);
  }
  return ac_pool_strdup(w->worker_pool, ac_buffer_data(bh));
}

size_t ac_worker_ram(ac_worker_t *w, double pct) {
  double total_ram = w->task->scheduler->ram;
  double running = w->running;
  running = running / pct;
  size_t res = (size_t)round((total_ram * pct) / running);
  return res * 1024;
}

static void fill_inputs(ac_worker_t *w) {
  if (w->inputs) {
    ac_worker_input_t *n = w->inputs;
    while (n) {
      n->num_files = 0;
      n->files = n->file_info(w, &(n->num_files), n);
      n = n->next;
    }
  }
}

ac_transform_t *clone_transforms(ac_worker_t *w) {
  ac_transform_t *head = NULL;
  ac_transform_t *tail = NULL;
  ac_transform_t *n = w->task->transforms;
  while (n) {
    if (!head)
      head = tail = ac_pool_dup(w->worker_pool, n, sizeof(*n));
    else
      tail = ac_pool_dup(w->worker_pool, n, sizeof(*n));

    tail->input = ac_worker_input(w, tail->input->id);
    tail->next = NULL;
    n = n->next;
  }
  return head;
}

void clone_inputs_and_outputs(ac_worker_t *w) {
  ac_worker_input_t *head = NULL;
  ac_worker_input_t *tail = NULL;
  ac_worker_input_t *n = w->task->inputs;
  while (n) {
    if (!head)
      head = tail = ac_pool_dup(w->worker_pool, n, sizeof(*n));
    else
      tail = ac_pool_dup(w->worker_pool, n, sizeof(*n));
    tail->next = NULL;
    n = n->next;
  }
  w->inputs = head;
  w->outputs = w->task->outputs;
  w->data = clone_transforms(w);
}

void *schedule_thread(void *arg) {
  ac_schedule_thread_t *t = (ac_schedule_thread_t *)arg;
  t->pool = ac_pool_init(65536);
  t->bh = ac_buffer_init(200);
  ac_pool_t *tmp_pool = ac_pool_init(65536);
  while (true) {
    ac_pool_clear(t->pool);
    ac_worker_t *w = get_next_worker(t);
    if (!w)
      break;

    w->worker_pool = t->pool;
    ac_pool_clear(tmp_pool);
    w->pool = tmp_pool;
    get_ack_time(w);
    clone_inputs_and_outputs(w);
    fill_inputs(w);

    if (worker_needs_to_run(w)) {
      setup_worker(w);
      if (!run_worker(w)) {
        destroy_worker(w);
        break;
      }
      if (t->scheduler->on_complete) {
        if (!t->scheduler->on_complete(w)) {
          destroy_worker(w);
          break;
        }
      }
      worker_complete(w, time(NULL));
      destroy_worker(w);
    } else
      worker_complete(w, w->ack_time);
  }
  ac_pool_destroy(t->pool);
  ac_pool_destroy(tmp_pool);
  ac_buffer_destroy(t->bh);
  mark_as_done(t->scheduler);
  return NULL;
}

static bool in_out_runner(ac_worker_t *w) {
  ac_transform_t *transforms = (ac_transform_t *)w->data;
  while (transforms) {
    ac_in_t *in = ac_worker_in(w, transforms->input->id);
    if (!in)
      return true;

    ac_out_t **outs = (ac_out_t **)ac_pool_calloc(
        w->worker_pool, sizeof(ac_out_t *) * transforms->num_outputs);
    for (size_t i = 0; i < transforms->num_outputs; i++)
      outs[i] = ac_worker_out(w, transforms->outputs[i]->id);

    if (transforms->create_data)
      w->transform_data = transforms->create_data(w);

    ac_io_record_t *r;
    if (transforms->runner) {
      while ((r = ac_in_advance(in)) != NULL)
        transforms->runner(w, r, outs);
    } else if (transforms->group_runner) {
      void *compare_arg = NULL;
      if (transforms->create_group_compare_arg)
        compare_arg = transforms->create_group_compare_arg(w);
      size_t num_r = 0;
      bool more_records = false;
      while ((r = ac_in_advance_group(in, &num_r, &more_records,
                                      transforms->group_compare,
                                      compare_arg)) != NULL)
        transforms->group_runner(w, r, num_r, outs);
      if (transforms->destroy_group_compare_arg)
        transforms->destroy_group_compare_arg(w, compare_arg);
    } else {
      size_t num_outs = transforms->num_outputs;
      ac_io_record_t *r;
      while ((r = ac_in_advance(in)) != NULL) {
        for (size_t i = 0; i < num_outs; i++)
          ac_out_write_record(outs[i], r->record, r->length);
      }
    }
    ac_in_destroy(in);
    if (transforms->next)
      in = ac_out_in(outs[0]);
    else
      ac_out_destroy(outs[0]);
    for (size_t i = 1; i < transforms->num_outputs; i++)
      ac_out_destroy(outs[i]);
    if (transforms->destroy_data)
      transforms->destroy_data(w, w->transform_data);

    transforms = transforms->next;
  }
  return true;
}

void ac_task_runner(ac_task_t *task, ac_worker_f runner) {
  task->runner = runner;
}

void ac_task_default_runner(ac_task_t *task) { task->runner = in_out_runner; }

void ac_schedule_run(ac_schedule_t *h, ac_worker_f on_complete) {
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

static bool is_worker_complete(ac_task_t *task, size_t partition) {
  if (partition >= task->num_partitions)
    return is_worker_complete(task, 0);

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
    if (!is_worker_complete(n->task, partition))
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
      if (is_worker_complete(d, i))
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
