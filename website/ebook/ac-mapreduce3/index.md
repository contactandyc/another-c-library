---
path: "/ac-mapreduce3"
posttype: "docs"
title: "AC's Map Reduce Part 3"
---

## Ordering tasks with a dataflow

In the last post, tasks were ordered by using dependencies.  While this can be useful, a much more useful approach would be to order tasks based upon data dependencies.  The scheduler has a built in data pipeline that allows dependencies between tasks to be automatically determined.  If data changes within the pipeline, downstream tasks which were dependent upon that data will automatically be run.  

Each task within the scheduler will typically have inputs and outputs and in general those inputs and outputs will be files.  Within a larger overall set of tasks, there will be inputs which originate outside the scheduler (content that has been produced from other systems).  Those should be the only inputs that need defined.  All other inputs should be artifacts and/or outputs from other tasks.

In the last post, ac\_task\_input\_files was explored and connecting input from outside ac\_schedule.  To create a data pipeline, ac\_task\_output is used.

```c
void ac_task_output(ac_task_t *task, const char *name, const char *destinations,
                    double out_ram_pct, double in_ram_pct, size_t flags);
```

In examples/mapreduce2/input\_data\_3.c, setup\_split doesn't output anything.  I'll change it to output a file named split.lz4
```c
bool setup_split(ac_task_t *task) {
  ac_task_input_files(task, "split_input", 0.1, get_input_files);
  ac_task_runner(task, do_nothing);
  return true;
}
```

changes to
```c
bool setup_split(ac_task_t *task) {
  ac_task_input_files(task, "split_input", 0.1, get_input_files);
  ac_task_output(task, "split.lz4", "first|multi", 0.9, 0.1, AC_OUTPUT_SPLIT);
  ac_task_runner(task, do_nothing);
  return true;
}
```

The ac\_task\_output line defines an output file named split.lz4 and two destinations for the file.  The dependency upon split can now be removed from setup\_first and setup\_multi.

```c
bool setup_first(ac_task_t *task) {
  ac_task_dependency(task, "split");
  ac_task_runner(task, do_nothing);
  return true;
}

...

bool setup_multi(ac_task_t *task) {
  ac_task_dependency(task, "all|partition|first|split");
  ac_task_runner(task, do_nothing);
  return true;
}
```

will change to
```c
bool setup_first(ac_task_t *task) {
  ac_task_runner(task, do_nothing);
  return true;
}

...

bool setup_multi(ac_task_t *task) {
  ac_task_dependency(task, "all|partition|first");
  ac_task_runner(task, do_nothing);
  return true;
}
```

```
$ make data_pipeline_1
$ ./data_pipeline_1 -s --dir ../mapreduce2 --ext c
task: split [0/2]
  reverse dependencies:  first[2] multi[2]
  custom runner
      input[0]: split_input (4)
        ../mapreduce2/input_data_3.c (3269)
        ../mapreduce2/input_data_1.c (1915)
        ../mapreduce2/start.c (671)
        ../mapreduce2/order_tasks.c (1221)
      output[0]: split.lz4 split
        destinations: multi[2] first[2]
        tasks/split_0/split_0.lz4
task: split [1/2]
  reverse dependencies:  first[2] multi[2]
  custom runner
      input[0]: split_input (1)
        ../mapreduce2/input_data_2.c (3137)
      output[0]: split.lz4 split
        destinations: multi[2] first[2]
        tasks/split_1/split_1.lz4
task: first [0/2]
  dependencies:  split[2]
  reverse dependencies:  multi[2] partition[2]
  custom runner
      input[0]: split.lz4 (2)
        tasks/split_0/split_0_0.lz4 (0)
        tasks/split_1/split_1_0.lz4 (0)
task: first [1/2]
  dependencies:  split[2]
  reverse dependencies:  multi[2] partition[2]
  custom runner
      input[0]: split.lz4 (2)
        tasks/split_0/split_0_1.lz4 (0)
        tasks/split_1/split_1_1.lz4 (0)
task: partition [0/2]
  dependencies:  first[2]
  reverse dependencies:  multi[2]
  reverse partial dependencies:  all[2]
  custom runner
task: all [0/2]
  reverse dependencies:  multi[2]
  partial dependencies:  partition[2]
  custom runner
task: partition [1/2]
  dependencies:  first[2]
  reverse dependencies:  multi[2]
  reverse partial dependencies:  all[2]
  custom runner
task: all [1/2]
  reverse dependencies:  multi[2]
  partial dependencies:  partition[2]
  custom runner
task: multi [0/2]
  dependencies:  first[2] partition[2] all[2] split[2]
  custom runner
      input[0]: split.lz4 (2)
        tasks/split_0/split_0_0.lz4 (0)
        tasks/split_1/split_1_0.lz4 (0)
task: multi [1/2]
  dependencies:  first[2] partition[2] all[2] split[2]
  custom runner
      input[0]: split.lz4 (2)
        tasks/split_0/split_0_1.lz4 (0)
        tasks/split_1/split_1_1.lz4 (0)
```

Both multi and first still depend upon split even though they don't call ac\_task\_dependency.  They both have an input and the input consists of two files.  The files come from both split partitions and the filenames a different for partition 0 and partition 1 of multi and first.  When AC\_OUTPUT\_SPLIT is defined in the ac\_task\_output function, the output from each partition is expected to be split into the number of partitions as the destination task (or tasks).  If the destination has more than one task in its list, all of the destinations must have the same number of partitions.

```
        split[0]                          split[1]
        /       \                         /       \
split_0_0.lz4   split_0_1.lz4    split_1_0.lz4    split_1_1.lz4
      |              |                 |               |
   first[0]        first[1]          first[0]        first[1]
   multi[0]        multi[1]          multi[0]        multi[1]

split_0_0.lz4   split_1_0.lz4    split_0_1.lz4   split_1_1.lz4
        \       /                        \       /
        first[0]                         first[1]
        multi[0]                         multi[1]
```

It's important at this point to note that these files aren't actually created.  They're expected to be created.  Our ac\_task\_runner is still calling do\_nothing which only returns true.  The next section will wire up the remaining tasks.

examples/mapreduce3/data\_pipeline\_1.c
```c
#include "ac_schedule.h"

typedef struct {
  size_t num_inputs;
  ac_io_file_info_t *inputs;
  char *dir;
  char *ext;
  char **extensions;
} custom_arg_t;

bool file_ok(const char *filename, void *arg) {
  custom_arg_t *ca = (custom_arg_t *)arg;
  char **p = ca->extensions;
  while (*p) {
    if (ac_io_extension(filename, *p))
      return true;
    p++;
  }
  return false;
}

bool finish_custom_args(int argc, char **argv, void *arg) {
  custom_arg_t *ca = (custom_arg_t *)arg;
  if (!ca->dir)
    ca->dir = (char *)"sample";
  if (!ca->ext)
    ca->ext = (char *)"tbontb";

  ca->extensions = ac_split2(NULL, ',', ca->ext);
  ca->inputs = ac_io_list(ca->dir, &ca->num_inputs, file_ok, ca);
  if (ca->inputs)
    return true;
  return false;
}

int parse_custom_args(int argc, char **argv, void *arg) {
  if (argc < 2)
    return 0;

  custom_arg_t *ca = (custom_arg_t *)arg;

  if (!strcmp(argv[0], "--dir")) {
    ca->dir = argv[1];
    return 2;
  } else if (!strcmp(argv[0], "--ext")) {
    ca->ext = argv[1];
    return 2;
  }
  return 0;
}

ac_io_file_info_t *get_input_files(ac_worker_t *w, size_t *num_files,
                                   ac_worker_input_t *inp) {
  custom_arg_t *ca = (custom_arg_t *)ac_task_custom_arg(w->task);
  return ac_io_select_file_info(w->worker_pool, num_files, ca->inputs,
                                ca->num_inputs, w->partition,
                                w->num_partitions);
}

bool do_nothing(ac_worker_t *w) { return true; }

bool setup_split(ac_task_t *task) {
  ac_task_input_files(task, "split_input", 0.1, get_input_files);
  ac_task_output(task, "split.lz4", "first|multi", 0.9, 0.1, AC_OUTPUT_SPLIT);
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_first(ac_task_t *task) {
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_partition(ac_task_t *task) {
  ac_task_dependency(task, "first");
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_all(ac_task_t *task) {
  ac_task_partial_dependency(task, "partition");
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_multi(ac_task_t *task) {
  ac_task_dependency(task, "all|partition|first");
  ac_task_runner(task, do_nothing);
  return true;
}

void custom_usage() {
  printf("Find all words ending in .h, .c, and .md and sort by\n");
  printf("frequency descending.\n\n");
  printf("--dir <dir> - directory to scan\n");
  printf("--ext <extensions> - comma delimited list of file extensions to "
         "consider");
}

int main(int argc, char *argv[]) {
  ac_schedule_t *scheduler = ac_schedule_init(argc - 1, argv + 1, 2, 4, 10);
  custom_arg_t custom;
  memset(&custom, 0, sizeof(custom));

  ac_schedule_custom_args(scheduler, custom_usage, parse_custom_args,
                          finish_custom_args, &custom);

  ac_schedule_task(scheduler, "split", true, setup_split);
  ac_schedule_task(scheduler, "partition", true, setup_partition);
  ac_schedule_task(scheduler, "first", true, setup_first);
  ac_schedule_task(scheduler, "all", true, setup_all);
  ac_schedule_task(scheduler, "multi", true, setup_multi);

  ac_schedule_run(scheduler, ac_worker_complete);

  if (custom.inputs)
    ac_free(custom.inputs);
  if (custom.extensions)
    ac_free(custom.extensions);

  ac_schedule_destroy(scheduler);
  return 0;
}
```
