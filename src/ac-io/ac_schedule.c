#include "another-c-library/ac_schedule.h"

#include "another-c-library/ac_allocator.h"
#include "another-c-library/ac_io.h"
#include "another-c-library/ac_map.h"

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

struct ac_schedule_allocs_s;
typedef struct ac_schedule_allocs_s ac_schedule_allocs_t;

struct ac_schedule_allocs_s {
  void *d;
  size_t length;
  ac_schedule_allocs_t *next;
};

struct ac_schedule_thread_s {
  pthread_t thread;
  ac_schedule_t *scheduler;
  ac_pool_t *pool;
  ac_buffer_t *bh;
  ac_schedule_allocs_t *allocs;
  size_t thread_id;
  size_t partition;
};

typedef struct {
  ac_task_t *task;
  bool *partitions;
} selected_task_t;

typedef struct {
  bool dump;
  bool prefix;
  bool list;
  bool show_files;
  bool help;
  bool only_run_selected;
  bool force;
  bool select_all;

  size_t ram;
  size_t cpus;

  ac_task_t *debug_task;
  size_t debug_partition;
  char *debug_path;
  bool debug_dump_input;

  char **files;
  selected_task_t *tasks;
  size_t num_tasks;
  size_t num_selected;
} parsed_args_t;

struct ac_schedule_s {
  ac_map_t *task_root;
  ac_pool_t *pool;
  ac_pool_t *tmp_pool;

  char **args;
  int argc;

  parse_args_cb parse_args;
  finish_args_cb finish_args;
  void *parse_args_arg;
  void (*custom_usage)();

  char *ack_dir;
  char *task_dir;

  size_t num_available;
  size_t num_tasks_to_run;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  size_t num_running;

  ac_schedule_thread_t *threads;
  bool started_threads;

  ac_worker_cb on_complete;
  bool done;

  size_t num_partitions;
  ac_task_state_t *state;

  ac_task_t *head;
  ac_task_t *tail;

  size_t ram;
  size_t cpus;
  size_t disk_space;

  parsed_args_t parsed_args;
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
  ac_worker_input_t **inputs;
  size_t num_inputs;
  ac_worker_output_t **outputs;
  size_t num_outputs;
  ac_io_runner_cb io_runner;
  ac_runner_cb runner;
  ac_group_runner_cb group_runner;
  ac_io_compare_cb group_compare;
  ac_worker_data_cb create_group_compare_arg;
  ac_destroy_worker_data_cb destroy_group_compare_arg;
  ac_worker_data_cb create_data;
  ac_destroy_worker_data_cb destroy_data;
  ac_transform_t *next;
};

struct ac_task_s {
  ac_map_t node;

  char *task_name;
  size_t num_partitions;

  bool do_nothing;
  bool run_everytime;

  ac_schedule_t *scheduler;

  ac_task_cb setup;
  ac_worker_cb runner;

  ac_transform_t *transforms;

  ac_task_input_link_t *current_input;
  ac_worker_output_t *current_output;

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

  bool *selected;

  ac_task_state_link_t *state_linkage;

  ac_task_t *next;
};

static bool is_worker_selected(ac_worker_t *w) {
  if (w->task->selected && w->task->selected[w->partition])
    return true;
  return false;
}

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

  size_t num_inputs = 0;
  char **inputs = ac_pool_split2(pool, &num_inputs, '|', inp);
  t->num_inputs = num_inputs;
  if (num_inputs) {
    t->inputs = (ac_worker_input_t **)ac_pool_calloc(
        pool, sizeof(ac_worker_input_t *) * (num_inputs + 1));
    for (size_t i = 0; i < num_inputs; i++) {
      t->inputs[i] = ac_task_find_input(task, inputs[i]);
      if (!t->inputs[i])
        abort();
    }
  }

  size_t num_outputs = 0;
  char **outputs = ac_pool_split2(pool, &num_outputs, '|', outp);
  t->num_outputs = num_outputs;
  if (num_outputs) {
    t->outputs = (ac_worker_output_t **)ac_pool_calloc(
        pool, sizeof(ac_worker_output_t *) * (num_outputs + 1));
    for (size_t i = 0; i < num_outputs; i++) {
      t->outputs[i] = ac_task_find_output(task, outputs[i]);
      if (!t->outputs[i])
        abort();
    }
  }

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
                       ac_runner_cb runner) {
  ac_transform_t *t = _ac_task_transform(task, inp, outp);
  t->runner = runner;
}

void ac_task_io_transform(ac_task_t *task, const char *inp, const char *outp,
                          ac_io_runner_cb runner) {
  ac_transform_t *t = _ac_task_transform(task, inp, outp);
  t->io_runner = runner;
}

void ac_task_group_transform(ac_task_t *task, const char *inp, const char *outp,
                             ac_group_runner_cb runner,
                             ac_io_compare_cb compare) {
  ac_transform_t *t = _ac_task_transform(task, inp, outp);
  t->group_runner = runner;
  t->group_compare = compare;
}

void ac_task_group_compare_arg(ac_task_t *task, ac_worker_data_cb create,
                               ac_destroy_worker_data_cb destroy) {
  ac_transform_t *n = task->transforms;
  while (n->next)
    n = n->next;
  n->create_group_compare_arg = create;
  n->destroy_group_compare_arg = destroy;
}

void ac_task_transform_data(ac_task_t *task, ac_worker_data_cb create,
                            ac_destroy_worker_data_cb destroy) {
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
  if (flags & AC_OUTPUT_SPLIT) {
    if (!o->ext_options.partition) {
      printf("%s from %s is configured\n  to be split, but does not specify a "
             "partition method!  Exiting early!\n",
             base_name, w->task->task_name);
      abort();
    }
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

static void add_worker_allocation(ac_worker_t *w, void *d, size_t len) {
  ac_schedule_allocs_t *a =
      (ac_schedule_allocs_t *)ac_pool_alloc(w->worker_pool, sizeof(*a));
  a->d = d;
  a->length = len;
  a->next = w->schedule_thread->allocs;
  w->schedule_thread->allocs = a;
}

char *ac_worker_read(ac_worker_t *w, size_t *num_records, char **endp,
                     size_t pos) {
  *num_records = 0;
  ac_worker_input_t *inp = ac_worker_input(w, pos);
  if (!inp)
    return NULL;

  if (inp->num_files != 1)
    return NULL;

  size_t len = 0;
  char *buf = ac_io_read_file(&len, inp->files[0].filename);
  if (endp)
    *endp = buf + len;
  if (inp->options.format == 0) {
    if (num_records) {
      char *p = buf;
      char *ep = p + len;
      size_t num = 0;
      while (p < ep) {
        uint32_t length = (*(uint32_t *)p);
        num++;
        p += sizeof(uint32_t) + length;
      }
      *num_records = num;
    }
  }
  if (inp->options.format < 0) {
    if (num_records) {
      char delim = -inp->options.format;
      delim--;
      char *p = buf;
      char *ep = p + len;
      size_t num = 0;
      while (p < ep) {
        if (*p == delim)
          num++;
        p++;
      }
      if (len > 0 && ep[-1] != delim)
        num++;
      *num_records = num;
    }
  } else {
    if (num_records)
      *num_records = len / inp->options.format;
  }
  if (buf)
    add_worker_allocation(w, buf, len);

  return buf;
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

void ac_schedule_custom_args(ac_schedule_t *h, void (*custom_usage)(),
                             parse_args_cb parse_args, finish_args_cb finish_args,
                             void *arg) {
  h->parse_args = parse_args;
  h->finish_args = finish_args;
  h->parse_args_arg = arg;
  h->custom_usage = custom_usage;
}

void *ac_task_custom_arg(ac_task_t *task) {
  return task->scheduler->parse_args_arg;
}

ac_schedule_t *ac_schedule_init(int argc, char **args, size_t num_partitions,
                                size_t cpus, size_t ram) {
  size_t disk_space = 1000; // not really used at the moment
  ac_pool_t *pool = ac_pool_init(32768);
  ac_schedule_t *h = (ac_schedule_t *)ac_pool_calloc(
      pool, sizeof(ac_schedule_t) + (num_partitions * sizeof(ac_task_state_t)));

  pthread_mutex_init(&(h->mutex), NULL);
  pthread_cond_init(&(h->cond), NULL);

  h->started_threads = false;
  h->num_partitions = num_partitions;
  h->state = (ac_task_state_t *)(h + 1);
  h->ram = ram * 1024;
  h->cpus = cpus;
  h->disk_space = disk_space * 1024;
  h->pool = pool;
  h->tmp_pool = ac_pool_init(4096);
  h->args = argc > 0 ? ac_pool_strdupan(pool, args, argc) : NULL;
  h->argc = argc > 0 ? argc : 0;
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

static ac_worker_t *create_worker(ac_pool_t *pool, ac_task_t *task,
                                  size_t partition) {
  ac_worker_t *w = (ac_worker_t *)ac_pool_calloc(pool, sizeof(ac_worker_t));
  w->task = task;
  w->partition = partition;
  w->num_partitions = task->num_partitions;
  w->ack_time = -1;
  w->__link = NULL;
  return w;
}

static ac_worker_input_t *_ac_task_input(ac_task_t *task, const char *name,
                                         ac_worker_output_t *src, double pct,
                                         ac_worker_file_info_cb file_info) {
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
    res[i].tag = i;
    ac_io_file_info(res + i);
  }
  *num_files = num_partitions;
  return res;
}

ac_io_file_info_t *file_info_normal(ac_worker_t *w, size_t *num_files,
                                    ac_worker_input_t *inp) {
  size_t num_partitions = inp->src->task->num_partitions;
  ac_io_file_info_t *res = (ac_io_file_info_t *)ac_pool_calloc(
      w->worker_pool, sizeof(*res) * num_partitions);
  for (size_t i = 0; i < num_partitions; i++) {
    res[i].filename = ac_worker_input_name(w, inp, i);
    res[i].tag = i;
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
  res->tag = 0;
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
  res->tag = 0;
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
  res->tag = w->partition;
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
      ac_task_link_t *tl =
          (ac_task_link_t *)ac_pool_alloc(h->pool, sizeof(*tl));
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

    ac_task_link_t *tl = (ac_task_link_t *)ac_pool_alloc(h->pool, sizeof(*tl));
    tl->task = node;
    tl->next = NULL;
    return tl;
  }
}

bool match_filename(const char *a, const char *b) {
  if (!strcmp(a, b))
    return true;
  size_t l1 = strlen(a);
  size_t l2 = strlen(b);
  if (l2 > l1)
    a += (l2 - l1);
  if (!strcmp(a, b))
    return true;
  return false;
}

static void dump_file(ac_worker_t *w, const char *filename,
                      ac_io_format_t format, ac_task_dump_cb dump,
                      void *dump_arg) {
  ac_in_options_t opts;
  ac_in_options_init(&opts);
  ac_in_options_format(&opts, format);
  ac_in_t *in = ac_in_init(filename, &opts);
  ac_io_record_t *r;
  ac_buffer_t *bh = ac_buffer_init(1000);
  size_t line = 1;
  bool print_extra = w->task->scheduler->parsed_args.prefix;
  const char *printed_name = filename;
  const char *slash = strrchr(filename, '/');
  if (slash)
    printed_name = slash + 1;

  while ((r = ac_in_advance(in)) != NULL) {
    ac_buffer_clear(bh);
    dump(w, r, bh, dump_arg);
    if (print_extra) {
      printf("%s:%lu\t", printed_name, line);
      line++;
    }
    if (ac_buffer_length(bh))
      printf("%s\n", ac_buffer_data(bh));
  }
  ac_buffer_destroy(bh);
  ac_in_destroy(in);
}

static void scan_output(ac_worker_t *w, char **files, size_t num_files) {
  ac_worker_input_t *inp = w->inputs;
  while (inp) {
    if (inp->dump) {
      for (size_t i = 0; i < num_files; i++) {
        if (!files[i])
          continue;
        for (size_t j = 0; j < inp->num_files; j++) {
          if (match_filename(files[i], inp->files[j].filename)) {
            ac_pool_clear(w->pool);
            dump_file(w, files[i], inp->options.format, inp->dump,
                      inp->dump_arg);
            files[i] = NULL;
            break;
          }
        }
      }
    }
    inp = inp->next;
  }

  ac_worker_output_t *o = w->outputs;
  while (o) {
    if (o->dump) {
      char *base_name = ac_worker_output_base(w, o);
      size_t num_partitions = 0;
      if ((o->flags & AC_OUTPUT_SPLIT) && o->ext_options.partition) {
        if (o->destinations)
          num_partitions = o->destinations->task->num_partitions;
        else
          num_partitions = o->task->scheduler->num_partitions;
      }
      for (size_t i = 0; i < num_files; i++) {
        if (!files[i])
          continue;
        if (!num_partitions && match_filename(files[i], base_name)) {
          ac_pool_clear(w->pool);
          dump_file(w, files[i], o->options.format, o->dump, o->dump_arg);
          files[i] = NULL;
        }
        if (!files[i])
          continue;
        for (size_t j = 0; j < num_partitions; j++) {
          ac_pool_clear(w->pool);
          char *filename =
              (char *)ac_pool_alloc(w->pool, strlen(base_name) + 20);
          ac_out_partition_filename(filename, base_name, j);
          if (match_filename(files[i], filename)) {
            dump_file(w, files[i], o->options.format, o->dump, o->dump_arg);
            files[i] = NULL;
            break;
          }
        }
      }
    }
    o = o->next;
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

  task->current_output = to;
  task->current_input = NULL;

  ac_task_link_t *n = to->destinations;
  while (n) {
    ac_worker_file_info_cb file_info = NULL;
    if (flags & AC_OUTPUT_SPLIT) {
      file_info = file_info_split;
      ac_task_dependency(n->task, task->task_name);
    } else if (flags & AC_OUTPUT_USE_FIRST) {
      file_info = file_info_first;
      ac_task_dependency(n->task, task->task_name);
    } else if (flags & AC_OUTPUT_PARTITION) {
      file_info = file_info_partition;
      ac_task_partial_dependency(n->task, task->task_name);
    } else {
      file_info = file_info_normal;
      ac_task_dependency(n->task, task->task_name);
    }
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
                         ac_worker_file_info_cb file_info) {
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
void ac_task_output_partition(ac_task_t *task, ac_io_partition_cb part,
                              void *arg) {
  if (!task->current_output)
    return;

  ac_out_ext_options_partition(&(task->current_output->ext_options), part, arg);
}

void ac_task_dump_text(ac_worker_t *w, ac_io_record_t *r, ac_buffer_t *bh,
                       void *arg) {
  ac_buffer_appends(bh, r->record);
}

void ac_task_output_dump(ac_task_t *task, ac_task_dump_cb dump, void *arg) {
  if (!task->current_output)
    return;

  task->current_output->dump = dump;
  task->current_output->dump_arg = arg;

  // ac_task_input_dump(task, dump, arg);
}

void ac_task_output_compare(ac_task_t *task, ac_io_compare_cb compare,
                            void *arg) {
  if (!task->current_output)
    return;

  ac_out_ext_options_compare(&(task->current_output->ext_options), compare,
                             arg);
  ac_task_input_compare(task, compare, arg);
}

void ac_task_output_intermediate_compare(ac_task_t *task,
                                         ac_io_compare_cb compare, void *arg) {
  if (!task->current_output)
    return;

  ac_out_ext_options_intermediate_compare(&(task->current_output->ext_options),
                                          compare, arg);
}

void ac_task_output_keep_first(ac_task_t *task) {
  if (!task->current_output)
    return;

  ac_out_ext_options_reducer(&(task->current_output->ext_options),
                             ac_io_keep_first, NULL);
  ac_task_input_keep_first(task);
}

void ac_task_output_reducer(ac_task_t *task, ac_io_reducer_cb reducer,
                            void *arg) {
  if (!task->current_output)
    return;

  ac_out_ext_options_reducer(&(task->current_output->ext_options), reducer,
                             arg);
  ac_task_input_reducer(task, reducer, arg);
}

void ac_task_output_intermediate_reducer(ac_task_t *task,
                                         ac_io_reducer_cb reducer, void *arg) {
  if (!task->current_output)
    return;

  ac_out_ext_options_intermediate_reducer(&(task->current_output->ext_options),
                                          reducer, arg);
}

void ac_task_output_group_size(ac_task_t *task, size_t num_per_group,
                               size_t start) {
  if (!task->current_output)
    return;

  ac_out_ext_options_intermediate_group_size(
      &(task->current_output->ext_options), num_per_group);
}

void ac_task_output_use_extra_thread(ac_task_t *task) {
  if (!task->current_output)
    return;

  ac_out_ext_options_use_extra_thread(&(task->current_output->ext_options));
}

void ac_task_output_dont_compress_tmp(ac_task_t *task) {
  if (!task->current_output)
    return;

  ac_out_ext_options_dont_compress_tmp(&(task->current_output->ext_options));
}

void ac_task_output_sort_before_partitioning(ac_task_t *task) {
  if (!task->current_output)
    return;

  ac_out_ext_options_sort_before_partitioning(
      &(task->current_output->ext_options));
}

void ac_task_output_sort_while_partitioning(ac_task_t *task) {
  if (!task->current_output)
    return;

  ac_out_ext_options_sort_while_partitioning(
      &(task->current_output->ext_options));
}

void ac_task_output_num_sort_threads(ac_task_t *task, size_t num_sort_threads) {
  if (!task->current_output)
    return;

  ac_out_ext_options_num_sort_threads(&(task->current_output->ext_options),
                                      num_sort_threads);
}

void ac_task_output_format(ac_task_t *task, ac_io_format_t format) {
  if (!task->current_output)
    return;

  ac_out_options_format(&(task->current_output->options), format);
  ac_task_input_format(task, format);
}

void ac_task_output_safe_mode(ac_task_t *task) {
  if (!task->current_output)
    return;

  ac_out_options_safe_mode(&(task->current_output->options));
}

void ac_task_output_write_ack_file(ac_task_t *task) {
  if (!task->current_output)
    return;

  ac_out_options_write_ack_file(&(task->current_output->options));
}

void ac_task_output_gz(ac_task_t *task, int level) {
  if (!task->current_output)
    return;

  ac_out_options_gz(&(task->current_output->options), level);
}

void ac_task_output_lz4(ac_task_t *task, int level, ac_lz4_block_size_t size,
                        bool block_checksum, bool content_checksum) {
  if (!task->current_output)
    return;

  ac_out_options_lz4(&(task->current_output->options), level, size,
                     block_checksum, content_checksum);
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

void ac_task_input_dump(ac_task_t *task, ac_task_dump_cb dump, void *arg) {
  ac_task_input_link_t *inp = task->current_input;
  while (inp) {
    inp->input->dump = dump;
    inp->input->dump_arg = arg;
    inp = inp->next;
  }
}

void ac_task_input_compare(ac_task_t *task, ac_io_compare_cb compare,
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

void ac_task_input_reducer(ac_task_t *task, ac_io_reducer_cb reducer,
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

static bool in_out_runner(ac_worker_t *w);

static void get_ack_time_for_task(ac_task_t *task) {
  for (size_t i = 0; i < task->num_partitions; i++) {
    time_t *ack = &(task->state_linkage[i].ack_time);
    if (*ack == -1) {
      char *filename =
          ac_pool_strdupf(task->scheduler->pool, "%s/%s_%lu",
                          task->scheduler->ack_dir, task->task_name, i);
      *ack = ac_io_modified(filename);
    }
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
    get_ack_time_for_task(n);
    if (n->setup) {
      n->setup(n);
      if (n->runner == in_out_runner && !n->transforms) {
        ac_worker_input_t *i = n->inputs;
        char *inp = NULL;
        if (i) {
          ac_buffer_clear(bh);
          while (i) {
            ac_buffer_appends(bh, i->name);
            if (i->next)
              ac_buffer_appendc(bh, '|');
            i = i->next;
          }
          inp = ac_strdup(ac_buffer_data(bh));
        }
        ac_worker_output_t *o = n->outputs;
        ac_buffer_clear(bh);
        while (o) {
          ac_buffer_appends(bh, o->name);
          if (o->next)
            ac_buffer_appendc(bh, '|');
          o = o->next;
        }

        ac_task_transform(n, inp, ac_buffer_data(bh), NULL);
        if (inp)
          ac_free(inp);
      }
    }
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

static bool is_schedule_running(ac_worker_t *w) {
  parsed_args_t *p = &(w->task->scheduler->parsed_args);
  if (p->dump || p->list || p->help)
    return false;
  return true;
}

static void write_ack(ac_worker_t *w) {
  if (!is_schedule_running(w))
    return;

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
  if (when > w->ack_time && when > 1)
    write_ack(w);

  // ac_worker_t *next = NULL;
  ac_schedule_t *scheduler = w->task->scheduler;
  pthread_mutex_lock(&(scheduler->mutex));
  size_t num_available = scheduler->num_available;
  if (is_worker_selected(w)) {
    scheduler->parsed_args.num_selected--;
    if (!scheduler->parsed_args.num_selected)
      scheduler->done = true;
  }
  mark_task_complete(w->__link, w->partition, when);
  // if (w->next_task && take_available(w->next_task, w->partition))
  //  next = take_worker(w->next_task, w->partition);
  if ((!num_available && scheduler->num_available) || scheduler->done)
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
  if (scheduler->done) {
    pthread_mutex_unlock(&(scheduler->mutex));
    return NULL;
  }
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
  size_t n = 0;
  ac_worker_input_t *inp = w->inputs;
  while (inp) {
    if (inp->compare) {
      ac_in_t *in = ac_worker_in(w, n);
      ac_buffer_t *bh = ac_buffer_init(1000);
      ac_io_record_t cr;
      ac_io_record_t *r;
      if ((r = ac_in_advance(in)) != NULL) {
        ac_buffer_set(bh, r->record, r->length);
        cr = *r;
        cr.record = ac_buffer_data(bh);
        while ((r = ac_in_advance(in)) != NULL) {
          if (inp->compare(&cr, r, inp->compare_arg) > 0) {
            printf("%s test failed!\n", inp->name);
            abort();
          }
          ac_buffer_set(bh, r->record, r->length);
          cr = *r;
          cr.record = ac_buffer_data(bh);
        }
      }
      ac_in_destroy(in);
      ac_buffer_destroy(bh);
    }
    n++;
    inp = inp->next;
  }

  bool r = true;
  w->timer = ac_timer_init(1);
  ac_timer_start(w->timer);
  if (w->task->runner)
    r = w->task->runner(w);
  ac_timer_stop(w->timer);
  ac_schedule_allocs_t *a = w->schedule_thread->allocs;
  while (a) {
    ac_free(a->d);
    a = a->next;
  }
  w->schedule_thread->allocs = NULL;
  return r;
}

time_t get_ack_time_for_task_and_partition(ac_task_t *task, size_t partition) {
  if (partition >= task->num_partitions)
    return 0;
  time_t ack_time = task->state_linkage[partition].ack_time;
  time_t completed = task->state_linkage[partition].completed;

  return ack_time > completed ? ack_time : completed;
}

static bool task_run_per_args(ac_worker_t *w) {
  ac_schedule_t *scheduler = w->task->scheduler;
  if (scheduler->parsed_args.only_run_selected) {
    if (scheduler->parsed_args.select_all)
      return true;
    if (w->task->selected && w->task->selected[w->partition])
      return true;
    return false;
  }
  if (!scheduler->parsed_args.select_all &&
      !scheduler->parsed_args.num_selected)
    return false;
  return true;
}

static bool worker_needs_to_run(ac_worker_t *w) {
  if (w->ack_time == 0)
    return true;

  if (w->task->run_everytime)
    return true;

  if (w->task->scheduler->parsed_args.force) {
    if (w->task->scheduler->parsed_args.select_all)
      return true;
    if (w->task->selected && w->task->selected[w->partition])
      return true;
  }

  ac_task_t *task = w->task;
  ac_task_link_t *link = task->dependencies;
  while (link) {
    for (size_t i = 0; i < link->task->num_partitions; i++) {
      time_t ack_time = get_ack_time_for_task_and_partition(link->task, i);
      if (ack_time > w->ack_time)
        return true;
    }
    link = link->next;
  }
  link = task->partial_dependencies;
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
  if (!outp)
    return NULL;
  // base_<part><suffix>
  ac_buffer_t *bh = w->schedule_thread->bh;
  if (w->task->scheduler->parsed_args.debug_path) {
    ac_buffer_sets(bh, w->task->scheduler->parsed_args.debug_path);
    if (ac_buffer_length(bh) > 0 &&
        ac_buffer_data(bh)[ac_buffer_length(bh) - 1] != '/')
      ac_buffer_appendc(bh, '/');
    if (ac_buffer_length(bh) == 0) {
      printf("[ERROR] Debug path seems to be invalid!\n");
      abort();
    }
  } else
    ac_buffer_setf(bh, "%s/%s_%lu/", w->task->scheduler->task_dir,
                   w->task->task_name, w->partition);
  const char *base = outp->name;
  if (ac_io_extension(base, "lz4")) {
    ac_buffer_append(bh, base, strlen(base) - 4);
    if (suffix)
      ac_buffer_appends(bh, suffix);
    ac_buffer_appendf(bh, "_%lu.lz4", w->partition);
  } else if (ac_io_extension(base, "gz")) {
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

char *ac_worker_input_params(ac_worker_t *w, size_t n) {
  ac_worker_input_t *inp = ac_worker_input(w, n);
  if (!inp)
    return (char *)"";
  if (inp->num_files == 1)
    return inp->files[0].filename;

  size_t len = 1;
  for (size_t i = 0; i < inp->num_files; i++) {
    len += strlen(inp->files[i].filename) + 1;
  }
  char *res = (char *)ac_pool_alloc(w->pool, len);
  char *p = res;
  for (size_t i = 0; i < inp->num_files; i++) {
    strcpy(p, inp->files[i].filename);
    p += strlen(inp->files[i].filename);
    if (i + 1 < inp->num_files)
      *p++ = ' ';
  }
  return res;
}

char *ac_worker_output_params(ac_worker_t *w, size_t n) {
  ac_worker_output_t *outp = ac_worker_output(w, n);
  if (!outp)
    return NULL;

  char *base = _ac_worker_output_base(w, outp, NULL);
  if (outp->flags & AC_OUTPUT_SPLIT) {
    size_t num_partitions = 0;
    if (outp->destinations)
      num_partitions = outp->destinations->task->num_partitions;
    else
      num_partitions = w->task->scheduler->num_partitions;

    return ac_pool_strdupf(w->pool, "%s %lu", base, num_partitions);
  } else
    return base;
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
  if (inp->src->flags & AC_OUTPUT_SPLIT) {
    if (ac_io_extension(base, "lz4")) {
      ac_buffer_append(bh, base, strlen(base) - 4);
      ac_buffer_appendf(bh, "_%lu_%lu.lz4", partition, w->partition);
    } else if (ac_io_extension(base, "gz")) {
      ac_buffer_append(bh, base, strlen(base) - 3);
      ac_buffer_appendf(bh, "_%lu_%lu.gz", partition, w->partition);
    } else {
      ac_buffer_appends(bh, base);
      ac_buffer_appendf(bh, "_%lu_%lu", partition, w->partition);
    }
  } else {
    if (ac_io_extension(base, "lz4")) {
      ac_buffer_append(bh, base, strlen(base) - 4);
      ac_buffer_appendf(bh, "_%lu.lz4", partition);
    } else if (ac_io_extension(base, "gz")) {
      ac_buffer_append(bh, base, strlen(base) - 3);
      ac_buffer_appendf(bh, "_%lu.gz", partition);
    } else {
      ac_buffer_appends(bh, base);
      ac_buffer_appendf(bh, "_%lu", partition);
    }
  }
  return ac_pool_strdup(w->worker_pool, ac_buffer_data(bh));
}

bool ac_worker_debug(ac_worker_t *w) {
  return w->task->scheduler->parsed_args.debug_task ? true : false;
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
      head = tail =
          (ac_transform_t *)ac_pool_dup(w->worker_pool, n, sizeof(*n));
    else {
      tail->next = (ac_transform_t *)ac_pool_dup(w->worker_pool, n, sizeof(*n));
      tail = tail->next;
    }
    if (tail->inputs) {
      tail->inputs = (ac_worker_input_t **)ac_pool_alloc(
          w->worker_pool, sizeof(ac_worker_input_t *) * tail->num_inputs);
      for (size_t i = 0; i < tail->num_inputs; i++)
        tail->inputs[i] = ac_worker_input(w, n->inputs[i]->id);
    }
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
      head = tail =
          (ac_worker_input_t *)ac_pool_dup(w->worker_pool, n, sizeof(*n));
    else {
      tail->next =
          (ac_worker_input_t *)ac_pool_dup(w->worker_pool, n, sizeof(*n));
      tail = tail->next;
    }
    tail->next = NULL;
    n = n->next;
  }
  w->inputs = head;
  w->outputs = w->task->outputs;
  w->data = clone_transforms(w);
}

void debug_task(ac_schedule_thread_t *h) {
  ac_pool_t *pool = ac_pool_init(65536);
  h->bh = ac_buffer_init(200);
  ac_pool_t *tmp_pool = ac_pool_init(65536);
  ac_buffer_t *bh = ac_buffer_init(1024);
  ac_worker_t *w = create_worker(pool, h->scheduler->parsed_args.debug_task,
                                 h->scheduler->parsed_args.debug_partition);
  w->running = 1;
  w->worker_pool = pool;
  w->pool = tmp_pool;
  w->bh = bh;
  w->schedule_thread = h;
  get_ack_time(w);
  clone_inputs_and_outputs(w);
  fill_inputs(w);

  setup_worker(w);
  run_worker(w);
  destroy_worker(w);
  ac_pool_destroy(pool);
  ac_pool_destroy(tmp_pool);
  ac_buffer_destroy(h->bh);
  ac_buffer_destroy(w->bh);
}

void *schedule_thread(void *arg) {
  ac_schedule_thread_t *t = (ac_schedule_thread_t *)arg;
  t->pool = ac_pool_init(65536);
  t->bh = ac_buffer_init(200);
  ac_pool_t *tmp_pool = ac_pool_init(65536);
  ac_buffer_t *bh = ac_buffer_init(1024);
  while (true) {
    ac_pool_clear(t->pool);
    ac_worker_t *w = get_next_worker(t);
    if (!w)
      break;

    w->worker_pool = t->pool;
    ac_pool_clear(tmp_pool);
    w->pool = tmp_pool;
    ac_buffer_clear(bh);
    w->bh = bh;
    get_ack_time(w);
    clone_inputs_and_outputs(w);
    fill_inputs(w);

    if (worker_needs_to_run(w)) {
      if (!task_run_per_args(w)) {
        worker_complete(w, w->ack_time ? w->ack_time : 1);
      } else {
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
      }
    } else
      worker_complete(w, w->ack_time ? w->ack_time : 1);
  }
  ac_pool_destroy(t->pool);
  ac_pool_destroy(tmp_pool);
  ac_buffer_destroy(bh);
  ac_buffer_destroy(t->bh);
  mark_as_done(t->scheduler);
  return NULL;
}

static void print_task_link(const char *s, ac_task_link_t *tl) {
  if (!tl)
    return;
  printf("%s", s);
  while (tl) {
    printf(" %s[%lu]", tl->task->task_name, tl->task->num_partitions);
    tl = tl->next;
  }
  printf("\n");
}

void list_selected_tasks(void *arg) {
  ac_schedule_thread_t *t = (ac_schedule_thread_t *)arg;
  t->pool = ac_pool_init(65536);
  t->bh = ac_buffer_init(200);
  ac_pool_t *tmp_pool = ac_pool_init(65536);
  ac_buffer_t *bh = ac_buffer_init(1024);
  bool show_files = t->scheduler->parsed_args.show_files;
  while (!t->scheduler->done) {
    ac_pool_clear(t->pool);
    ac_worker_t *w = get_next_worker(t);
    if (!w)
      break;

    w->worker_pool = t->pool;
    ac_pool_clear(tmp_pool);
    w->pool = tmp_pool;
    ac_buffer_clear(bh);
    w->bh = bh;

    get_ack_time(w);
    if ((w->task->scheduler->parsed_args.select_all ||
         (w->task->selected && w->task->selected[w->partition])) &&
        (show_files || w->partition == (w->num_partitions - 1))) {
      clone_inputs_and_outputs(w);
      fill_inputs(w);
      printf("task: %s [%lu/%lu]\n", w->task->task_name, w->partition,
             w->task->num_partitions);

      print_task_link("  dependencies: ", w->task->dependencies);
      print_task_link("  reverse dependencies: ",
                      w->task->reverse_dependencies);
      print_task_link("  partial dependencies: ",
                      w->task->partial_dependencies);
      print_task_link("  reverse partial dependencies: ",
                      w->task->reverse_partial_dependencies);

      if (w->task->runner == in_out_runner) {
        size_t id = 0;
        printf("  in_out_runner\n");
        ac_transform_t *transforms = (ac_transform_t *)w->data;
        while (transforms) {
          printf("    transform: %lu\n", id);
          id++;
          size_t num_outs = transforms->num_outputs;
          size_t num_ins = transforms->num_inputs;
          for (size_t i = 0; i < num_ins; i++) {
            ac_worker_input_t *n = transforms->inputs[i];
            printf("      input[%lu]: %s (%lu)\n", i, n->name, n->num_files);
            if (show_files) {
              for (size_t j = 0; j < n->num_files; j++)
                printf("        %s (%lu)\n", n->files[j].filename,
                       n->files[j].size);
            }
          }
          for (size_t i = 0; i < num_outs; i++) {
            ac_worker_output_t *n = transforms->outputs[i];
            printf("      output[%lu]: %s %s%s%s\n", i, n->name,
                   n->flags & AC_OUTPUT_SPLIT ? "split" : "",
                   n->flags & AC_OUTPUT_PARTITION ? "partition" : "",
                   (n->flags & (AC_OUTPUT_SPLIT | AC_OUTPUT_PARTITION))
                       ? ""
                       : "normal");
            ac_task_link_t *d = n->destinations;
            if (d) {
              printf("        destinations:");
              while (d) {
                printf(" %s[%lu]", d->task->task_name, d->task->num_partitions);
                d = d->next;
              }
              printf("\n");
            }
            if (show_files) {
              char *base_name = ac_worker_output_base(w, n);
              size_t num_partitions = 0;
              if ((n->flags & AC_OUTPUT_SPLIT) && n->ext_options.partition) {
                if (n->destinations)
                  num_partitions = n->destinations->task->num_partitions;
                else
                  num_partitions = n->task->scheduler->num_partitions;
              }
              if (!num_partitions)
                printf("        %s\n", base_name);
              char *filename =
                  (char *)ac_pool_alloc(w->pool, strlen(base_name) + 20);
              for (size_t j = 0; j < num_partitions; j++) {
                ac_out_partition_filename(filename, base_name, j);
                printf("        %s\n", filename);
              }
            }
          }
          transforms = transforms->next;
        }
      } else {
        printf("  custom runner\n");
        {
          ac_worker_input_t *n = w->inputs;
          size_t i = 0;
          while (n) {
            printf("      input[%lu]: %s (%lu)\n", i, n->name, n->num_files);
            i++;
            if (show_files) {
              for (size_t j = 0; j < n->num_files; j++)
                printf("        %s (%lu)\n", n->files[j].filename,
                       n->files[j].size);
            }
            n = n->next;
          }
        }
        {
          ac_worker_output_t *n = w->outputs;
          size_t i = 0;
          while (n) {
            printf("      output[%lu]: %s %s%s%s\n", i, n->name,
                   n->flags & AC_OUTPUT_SPLIT ? "split" : "",
                   n->flags & AC_OUTPUT_PARTITION ? "partition" : "",
                   (n->flags & (AC_OUTPUT_SPLIT | AC_OUTPUT_PARTITION))
                       ? ""
                       : "normal");
            ac_task_link_t *d = n->destinations;
            if (d) {
              printf("        destinations:");
              while (d) {
                printf(" %s[%lu]", d->task->task_name, d->task->num_partitions);
                d = d->next;
              }
              printf("\n");
            }
            if (show_files) {
              char *base_name = ac_worker_output_base(w, n);
              size_t num_partitions = 0;
              if ((n->flags & AC_OUTPUT_SPLIT) && n->ext_options.partition) {
                if (n->destinations)
                  num_partitions = n->destinations->task->num_partitions;
                else
                  num_partitions = n->task->scheduler->num_partitions;
              }
              if (!num_partitions)
                printf("        %s\n", base_name);
              char *filename =
                  (char *)ac_pool_alloc(w->pool, strlen(base_name) + 20);
              for (size_t j = 0; j < num_partitions; j++) {
                ac_out_partition_filename(filename, base_name, j);
                printf("        %s\n", filename);
              }
            }
            n = n->next;
          }
        }
      }
    }

    worker_complete(w, time(NULL));
  }
  ac_pool_destroy(t->pool);
  ac_pool_destroy(tmp_pool);
  ac_buffer_destroy(bh);
  ac_buffer_destroy(t->bh);
  mark_as_done(t->scheduler);
}

void ac_worker_dump_input(ac_worker_t *w, ac_io_record_t *r, size_t n) {
  ac_worker_input_t *inp = ac_worker_input(w, n);
  if (inp && inp->dump) {
    ac_buffer_clear(w->bh);
    inp->dump(w, r, w->bh, inp->dump_arg);
    printf("%s\n", ac_buffer_data(w->bh));
  } else if (inp && inp->src->dump) {
    ac_buffer_clear(w->bh);
    inp->src->dump(w, r, w->bh, inp->src->dump_arg);
    printf("%s\n", ac_buffer_data(w->bh));
  }
}

void dump_selected_tasks(void *arg) {
  ac_schedule_thread_t *t = (ac_schedule_thread_t *)arg;
  t->pool = ac_pool_init(65536);
  t->bh = ac_buffer_init(200);
  ac_pool_t *tmp_pool = ac_pool_init(65536);
  ac_buffer_t *bh = ac_buffer_init(1024);
  char **files = t->scheduler->parsed_args.files;
  size_t num_files = 0;
  for (size_t i = 0; files[i] != NULL; i++)
    num_files++;
  if (!num_files)
    return;

  if (t->scheduler->parsed_args.debug_task) {
    ac_worker_t *w =
        create_worker(t->pool, t->scheduler->parsed_args.debug_task,
                      t->scheduler->parsed_args.debug_partition);
    w->running = 1;
    w->worker_pool = t->pool;
    w->pool = tmp_pool;
    w->bh = bh;
    w->schedule_thread = t;
    get_ack_time(w);
    clone_inputs_and_outputs(w);
    fill_inputs(w);
    scan_output(w, files, num_files);
  } else {
    while (!t->scheduler->done) {
      ac_pool_clear(t->pool);
      ac_worker_t *w = get_next_worker(t);
      if (!w)
        break;

      w->worker_pool = t->pool;
      ac_pool_clear(tmp_pool);
      w->pool = tmp_pool;
      ac_buffer_clear(bh);
      w->bh = bh;

      get_ack_time(w);
      clone_inputs_and_outputs(w);
      fill_inputs(w);
      scan_output(w, files, num_files);
      worker_complete(w, time(NULL));
    }
  }
  ac_pool_destroy(t->pool);
  ac_pool_destroy(tmp_pool);
  ac_buffer_destroy(bh);
  ac_buffer_destroy(t->bh);
  if (!t->scheduler->parsed_args.debug_task)
    mark_as_done(t->scheduler);
}

static bool in_out_runner(ac_worker_t *w) {
  ac_transform_t *transforms = (ac_transform_t *)w->data;
  ac_in_t *in = NULL;
  while (transforms) {
    size_t num_outs = transforms->num_outputs;
    size_t num_ins = transforms->num_inputs + (in ? 1 : 0);
    ac_in_t **ins = NULL;
    ac_worker_input_t *inp = NULL;
    if (!num_ins) {
      ins = (ac_in_t **)ac_pool_calloc(w->worker_pool, sizeof(ac_in_t *));
      ins[0] = ac_in_empty();
    } else {
      ins = (ac_in_t **)ac_pool_calloc(w->worker_pool,
                                       sizeof(ac_in_t *) * num_ins);
      size_t ibase = 0;
      if (in) {
        ins[0] = in;
        ibase = 1;
      } else
        inp = ac_worker_input(w, transforms->inputs[0]->id);
      for (size_t i = ibase; i < num_ins; i++)
        ins[i] = ac_worker_in(w, transforms->inputs[i - ibase]->id);
    }
    ac_out_t **outs = (ac_out_t **)ac_pool_calloc(
        w->worker_pool, sizeof(ac_out_t *) * num_outs);
    for (size_t i = 0; i < num_outs; i++)
      outs[i] = ac_worker_out(w, transforms->outputs[i]->id);

    if (transforms->create_data)
      w->transform_data = transforms->create_data(w);

    ac_io_record_t *r;
    if (num_ins == 1) {
      if (transforms->runner) {
        while ((r = ac_in_advance(ins[0])) != NULL) {
          ac_pool_clear(w->pool);
          transforms->runner(w, r, outs);
        }
      } else if (transforms->group_runner) {
        void *compare_arg = NULL;
        if (transforms->create_group_compare_arg)
          compare_arg = transforms->create_group_compare_arg(w);
        size_t num_r = 0;
        bool more_records = false;
        while ((r = ac_in_advance_group(ins[0], &num_r, &more_records,
                                        transforms->group_compare,
                                        compare_arg)) != NULL) {
          ac_pool_clear(w->pool);
          transforms->group_runner(w, r, num_r, outs);
        }
        if (transforms->destroy_group_compare_arg)
          transforms->destroy_group_compare_arg(w, compare_arg);
      } else if (transforms->io_runner) {
        transforms->io_runner(w, ins, num_ins, outs, num_outs);
      } else {
        ac_io_record_t *r;
        if (inp && inp->src && inp->src->dump &&
            w->task->scheduler->parsed_args.debug_dump_input) {
          ac_buffer_t *bh = ac_buffer_init(1000);
          while ((r = ac_in_advance(ins[0])) != NULL) {
            ac_buffer_clear(bh);
            inp->src->dump(w, r, bh, inp->dump_arg);
            if (ac_buffer_length(bh))
              printf("%s\n", ac_buffer_data(bh));
            for (size_t i = 0; i < num_outs; i++)
              ac_out_write_record(outs[i], r->record, r->length);
          }
          ac_buffer_destroy(bh);
        } else {
          while ((r = ac_in_advance(ins[0])) != NULL) {
            for (size_t i = 0; i < num_outs; i++)
              ac_out_write_record(outs[i], r->record, r->length);
          }
        }
      }
    } else {
      if (transforms->io_runner)
        transforms->io_runner(w, ins, num_ins, outs, num_outs);
      else
        abort();
    }
    for (size_t i = 0; i < num_ins; i++)
      ac_in_destroy(ins[i]);
    in = NULL;
    if (num_outs > 0) {
      if (transforms->next)
        in = ac_out_in(outs[0]);
      else
        ac_out_destroy(outs[0]);
      for (size_t i = 1; i < num_outs; i++)
        ac_out_destroy(outs[i]);
    }
    if (transforms->destroy_data)
      transforms->destroy_data(w, w->transform_data);

    transforms = transforms->next;
  }
  return true;
}

void ac_task_runner(ac_task_t *task, ac_worker_cb runner) {
  task->runner = runner;
}

void ac_task_default_runner(ac_task_t *task) { task->runner = in_out_runner; }

bool ac_worker_complete(ac_worker_t *w) {
  fprintf(stderr, "Finished %s[%lu] on thread %lu in %0.3fms\n",
          ac_task_name(w->task), w->partition, w->thread_id,
          ac_timer_ms(w->timer));
  return true;
}

static void schedule_usage(ac_schedule_t *h) {
  if (h->custom_usage) {
    h->custom_usage();
    printf("\n----------------------------------------------------------\n\n");
  }
  printf("The scheduler is meant to aid in running tasks in parallel.\n");
  printf("At the moment, it operates on a single host - but I'm planning\n");
  printf("on improving it to support multiple computers.\n");
  printf("\n\n");
  printf("--debug <task:partition> <output path> - run a single task in\n");
  printf("   isolated environment\n\n");
  printf("-f|--force rerun selected tasks even if they don't need run\n\n");
  printf("-t <task[:partitions]>[<task[:partitions]], select tasks and\n");
  printf("   optionally partitions.  tasks are separated by vertical bars\n");
  printf(
      "   (|) and partitions are sub-selected by placing a colon and then\n");
  printf("   the partitions.  The partitions can be a single partition\n");
  printf("   number, arange separated by a - (1-3), or a list of single\n");
  printf(
      "   partitions or ranges separated by commas.  To select partitions\n");
  printf("   1, 3, 4, and 5 of task named first_task\n");
  printf("   -t first_task:1,3-5\n\n");
  printf("-o  Normally, all tasks that are needed to run to complete\n");
  printf("    selected task will run.  This will override that behavior\n");
  printf("    and only run selected tasks\n\n");
  printf(
      "-d|--dump <filename1,[filename2],...> dump the contents of files\n\n");
  printf(
      "-p|--prefix <filename1,[filename2],...> dump the contents of files\n");
  printf("    and prefix each line with the line number\n\n");
  printf("-l|--list list details of execution (the plan)\n\n");
  printf("-s|--show-files similar to list, except input and output files\n");
  printf("     are also displayed.\n\n");
  printf("-c|--cpus <num_cpus> overrides default number of cpus\n\n");
  printf("-r|--ram <ram MB> overrides default ram usage\n\n");
  printf("-h|--help show this help message\n\n");
}

void parse_args(ac_schedule_t *h) {
  parsed_args_t at;
  memset(&at, 0, sizeof(at));

  char *filenames = NULL;
  char *tasks = NULL;

  char *debug_task = NULL;

  char **p = h->args;
  char **ep = p + h->argc;
  bool should_read_args = true;
  char **custom_args =
      (char **)ac_pool_calloc(h->pool, sizeof(char *) * (h->argc + 1));
  size_t num_custom_args = 0;

  while (p < ep) {
    if (!strcmp(*p, "-f") || !(strcmp(*p, "--force"))) {
      at.force = true;
      p++;
    } else if (!strcmp(*p, "--debug")) {
      p++;
      if (p + 2 <= ep) {
        debug_task = *p;
        p++;
        at.debug_path = *p;
        p++;
        if (p + 1 <= ep) {
          if (!strcmp(*p, "dump_input")) {
            at.debug_dump_input = true;
            p++;
          }
        }
      } else {
        at.help = true;
        p = ep;
      }
    } else if (!strcmp(*p, "-o")) {
      at.only_run_selected = true;
      p++;
    } else if (!strcmp(*p, "--new-args")) {
      should_read_args = false;
      p++;
    } else if (!strcmp(*p, "-l") || !(strcmp(*p, "--list"))) {
      at.list = true;
      at.dump = false;
      at.prefix = false;
      p++;
    } else if (!strcmp(*p, "-s") || !(strcmp(*p, "--show-files"))) {
      at.list = true;
      at.show_files = true;
      at.dump = false;
      at.prefix = false;
      p++;
    } else if (!strcmp(*p, "-h") || !(strcmp(*p, "--help"))) {
      at.help = true;
      p++;
    } else if (!strcmp(*p, "-d") || !(strcmp(*p, "--dump")) ||
               !strcmp(*p, "-p") || !(strcmp(*p, "--prefix"))) {
      at.list = false;
      at.show_files = false;
      at.dump = true;
      char *ptr = *p;
      if (ptr[1] == 'p' || (ptr[1] == '-' && ptr[2] == 'p'))
        at.prefix = true;
      p++;
      if (p == ep || (*p)[0] == '-') {
        printf("[ERROR] --dump and --prefix requires one or more files to be "
               "listed after parameter\n\n");
        at.help = true;
      } else {
        while (p < ep && (*p)[0] != '-') {
          filenames =
              ac_pool_strdupf(h->pool, "%s%s%s", filenames ? filenames : "",
                              filenames ? "," : "", *p);
          p++;
        }
      }
    } else if (!strcmp(*p, "-t") || !(strcmp(*p, "--task"))) {
      p++;
      if (p == ep || (*p)[0] == '-') {
        printf("[ERROR] --task requires one or more tasks to be listed after "
               "parameter\n\n");
        at.help = true;
      } else {
        while (p < ep && (*p)[0] != '-') {
          tasks = ac_pool_strdupf(h->pool, "%s%s%s", tasks ? tasks : "",
                                  tasks ? "|" : "", *p);
          p++;
        }
      }
    } else if (!strcmp(*p, "-r") || !strcmp(*p, "--ram")) {
      p++;
      if (p == ep) {
        printf("[ERROR] --ram requires MB to be listed after parameter\n\n");
        at.help = true;
      } else {
        if (sscanf(*p, "%lu", &at.ram) != 1)
          at.help = true;
        p++;
      }
    } else if (!strcmp(*p, "-c") || !strcmp(*p, "--cpus")) {
      p++;
      if (p == ep) {
        printf("[ERROR] --cpus requires number of cpus to be listed after "
               "parameter\n\n");
        at.help = true;
      } else {
        if (sscanf(*p, "%lu", &at.cpus) != 1)
          at.help = true;
        p++;
      }
    } else {
      if (h->parse_args) {
        int n = h->parse_args(ep - p, p, h->parse_args_arg);
        if (n <= 0) {
          printf("[ERROR] Invalid custom argument %s\n\n", *p);
          at.help = true;
          p = ep;
        } else {
          while (n) {
            custom_args[num_custom_args] = *p;
            p++;
            num_custom_args++;
            n--;
          }
        }
      } else {
        printf("[ERROR] Invalid argument %s\n\n", *p);
        at.help = true;
        p = ep;
      }
    }
  }
  if (h->parse_args) {
    char *custom_args_file =
        ac_pool_strdupf(h->pool, "%s/custom_args", h->task_dir);
    char **old_args = NULL;
    size_t num_old_args = 0;
    if (should_read_args) {
      size_t len;
      char *buf = ac_io_read_file(&len, custom_args_file);
      if (buf) {
        old_args = ac_pool_split(h->pool, &num_old_args, '\n', buf);
        ac_free(buf);
      }
    }

    /* compare old args with new args */
    if (!num_old_args) {
      // go ahead and write new args
      if (num_custom_args) {
        FILE *out = fopen(custom_args_file, "wb");
        for (size_t i = 0; i < num_custom_args; i++)
          fprintf(out, "%s\n", custom_args[i]);
        fclose(out);
      }
    } else if (!num_custom_args) {
      custom_args = old_args;
      num_custom_args = num_old_args;
      p = custom_args;
      ep = p + num_custom_args;
      while (p < ep) {
        int n = h->parse_args(ep - p, p, h->parse_args_arg);
        if (n <= 0) {
          printf("[ERROR] Invalid custom argument %s\n\n", *p);
          at.help = true;
          p = ep;
        } else
          p += n;
      }
    } else {
      p = old_args;
      ep = p + num_old_args;
      char **p2 = custom_args;
      char **ep2 = p2 + num_custom_args;
      while (p < ep && p2 < ep2) {
        if (strcmp(*p, *p2)) {
          printf("[ERROR] Command line arguments don\'t match (%s != %s) - "
                 "(use --new-args?)\n\n",
                 *p, *p2);
          at.help = true;
          break;
        }
        p++;
        p2++;
      }
      if (!at.help) {
        if (p < ep) {
          printf("[ERROR] Command line arguments don\'t match (more old args "
                 "%s(%lu) "
                 "than new (%lu)) - (use --new-args?)\n\n",
                 *p, num_old_args, num_custom_args);
          at.help = true;
        } else if (p2 < ep2) {
          printf("[ERROR] Command line arguments don\'t match (more new args "
                 "%s(%lu) "
                 "than old (%lu)) - (use --new-args?)\n\n",
                 *p2, num_old_args, num_custom_args);
          at.help = true;
        }
      }
    }
  }

  at.num_selected = 0;
  at.select_all = true;
  at.files = ac_pool_split(h->pool, NULL, ',', filenames);
  if (!at.help && tasks) {
    at.select_all = false;
    size_t num_t = 0;
    char **t = ac_pool_split(h->pool, &num_t, '|', tasks);
    if (num_t) {
      selected_task_t *selected = (selected_task_t *)ac_pool_calloc(
          h->pool, sizeof(selected_task_t) * num_t);
      at.tasks = selected;
      at.num_tasks = num_t;
      for (size_t i = 0; i < num_t; i++) {
        char *p = strchr(t[i], ':');
        if (p) {
          *p++ = 0;
        }
        selected[i].task = find_task(h, t[i]);
        if (!selected[i].task) {
          at.help = true;
          break;
        }
        size_t num_partitions = selected[i].task->num_partitions;
        selected[i].partitions =
            (bool *)ac_pool_calloc(h->pool, sizeof(bool) * num_partitions);
        if (p) {
          char **parts = &p;
          char **eparts = parts + 1;

          if (strchr(p, ',')) {
            size_t num_parts = 0;
            parts = ac_pool_split(h->pool, &num_parts, ',', p);
            eparts = parts + num_parts;
          }
          while (!at.help && parts < eparts) {
            p = *parts;
            parts++;
            if (strchr(p, '-')) {
              size_t p1, p2;
              if (sscanf(p, "%lu-%lu", &p1, &p2) != 2 || p1 >= num_partitions ||
                  p2 >= num_partitions || p2 < p1) {
                at.help = true;
                break;
              } else {
                while (p1 <= p2) {
                  if (!selected[i].partitions[p1]) {
                    selected[i].partitions[p1] = true;
                    at.num_selected++;
                  }
                  p1++;
                }
              }
            } else {
              size_t p1;
              if (sscanf(p, "%lu", &p1) != 1 || p1 >= num_partitions) {
                at.help = true;
                break;
              } else {
                if (!selected[i].partitions[p1]) {
                  selected[i].partitions[p1] = true;
                  at.num_selected++;
                }
                p1++;
              }
            }
          }
        } else {
          for (size_t p1 = 0; p1 < num_partitions; p1++) {
            if (!selected[i].partitions[p1]) {
              selected[i].partitions[p1] = true;
              at.num_selected++;
            }
          }
        }
      }
    }
  }
  if (!at.help && at.tasks) {
    for (size_t i = 0; i < at.num_tasks; i++)
      at.tasks[i].task->selected = at.tasks[i].partitions;
  }
  if (!at.help && debug_task) {
    debug_task = ac_pool_strdup(h->pool, debug_task);
    char *p = strchr(debug_task, ':');
    if (!p) {
      printf("[ERROR]: debug task partition not specified %s\n\n", debug_task);
      at.help = true;
    } else {
      *p++ = 0;
      at.debug_task = find_task(h, debug_task);
      if (!at.debug_task) {
        at.help = true;
        printf("[ERROR]: debug task not found %s\n\n", debug_task);
      } else {
        if (sscanf(p, "%lu", &at.debug_partition) != 1 ||
            at.debug_partition >= at.debug_task->num_partitions) {
          printf("[ERROR]: debug task partition not valid %s\n\n", p);
          at.help = true;
        } else
          ac_io_make_directory(at.debug_path);
      }
      if (!at.help) {
        h->parsed_args.cpus = 1;
      }
    }
  }
  h->parsed_args = at;
}

void ac_schedule_run(ac_schedule_t *h, ac_worker_cb on_complete) {
  schedule_setup(h);
  parse_args(h);
  if (h->parsed_args.cpus) {
    h->cpus = h->parsed_args.cpus;
    // TODO: fix this so that you can have more cpus
    // if (h->cpus > h->num_partitions)
    //   h->cpus = h->num_partitions;
  }
  if (h->parsed_args.ram)
    h->ram = h->parsed_args.ram * 1024;

  if (h->parsed_args.help || h->parsed_args.dump || h->parsed_args.list)
    h->cpus = 1;

  h->threads = (ac_schedule_thread_t *)ac_pool_calloc(
      h->pool, sizeof(ac_schedule_thread_t) * h->cpus);
  for (size_t i = 0; i < h->cpus; i++) {
    ac_schedule_thread_t *a = h->threads + i;
    a->thread_id = i;
    a->scheduler = h;
  }

  if (h->parsed_args.debug_task) {
    if (h->parsed_args.dump) {
      h->num_running = h->cpus;
      h->on_complete = NULL;
      h->done = false;
      dump_selected_tasks(h->threads + 0);
    } else
      debug_task(h->threads + 0);
    return;
  }

  if (h->finish_args) {
    if (!h->finish_args(h->argc, h->args, h->parse_args_arg))
      h->parsed_args.help = true;
  }

  if (h->parsed_args.help) {
    schedule_usage(h);
    return;
  } else if (h->parsed_args.dump) {
    h->num_running = h->cpus;
    h->on_complete = NULL;
    h->done = false;
    dump_selected_tasks(h->threads);
  } else if (h->parsed_args.list) {
    h->num_running = h->cpus;
    h->on_complete = NULL;
    h->done = false;
    list_selected_tasks(h->threads);
  } else {
    h->num_running = h->cpus;
    h->on_complete = on_complete;
    h->done = false;
    for (size_t i = 0; i < h->cpus; i++)
      pthread_create(&(h->threads[i].thread), NULL, schedule_thread,
                     h->threads + i);
    for (size_t i = 0; i < h->cpus; i++)
      pthread_join(h->threads[i].thread, NULL);
  }
}

ac_task_t *ac_schedule_task(ac_schedule_t *h, const char *task_name,
                            bool partitioned, ac_task_cb setup) {
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
