---
path: "/ac-mapreduce2"
posttype: "docs"
title: "AC's Map Reduce Part 2"
---

## Introducing ac\_schedule

In the previous AC map/reduce post, the ac\_in/ac\_out objects were used to locate files based upon a directory and one or more extensions, read those the files a line at a time, lowercase each line, tokenize each line, sort the tokens alphabetically, reduce the frequencies, and finally sort by frequency descending followed by a secondary sort of tokens ascending.  

Part two introduces ac\_schedule.  While the previous code is efficient, my macbook has 16 threaded cores and a lot of RAM.  The code in part one is single threaded.  ac\_schedule let's you break up your work into smaller pieces, connect the work together, and it handles all of the execution.  Part two builds upon part one (part one should not be skipped).

The ac\_in/ac\_out could easily be used to split up data manually and made to be multithreaded.  The ac\_schedule object does this for you and a lot more.  It manages tasks, dependency chains, file handling, file naming, single task debugging, and so much more.

## Setting up ac\_schedule

The ac\_schedule isn't particularly complicated, it just has a few parts to it.  The first thing to do is setup tasks.  For now, these tasks will do nothing at all (other than return true indicating that they didn't fail).

examples/mapreduce2/start.c
```c
#include "ac_schedule.h"

bool do_nothing(ac_worker_t *w) { return true; }

bool setup_task(ac_task_t *task) {
  ac_task_runner(task, do_nothing);
  return true;
}

int main(int argc, char *argv[]) {
  ac_schedule_t *scheduler = ac_schedule_init(argc - 1, argv + 1, 2, 4, 10);

  ac_schedule_task(scheduler, "split", false, setup_task);
  ac_schedule_task(scheduler, "partition", true, setup_task);
  ac_schedule_task(scheduler, "first", true, setup_task);
  ac_schedule_task(scheduler, "all", true, setup_task);
  ac_schedule_task(scheduler, "multi", true, setup_task);

  ac_schedule_run(scheduler, ac_worker_complete);

  ac_schedule_destroy(scheduler);
  return 0;
}
```

```
$ make start
$ ./start
Finished multi[0] on thread 0 in 0.000ms
Finished first[0] on thread 2 in 0.000ms
Finished all[0] on thread 1 in 0.000ms
Finished partition[0] on thread 3 in 0.000ms
Finished split[0] on thread 0 in 0.000ms
Finished multi[1] on thread 2 in 0.000ms
Finished all[1] on thread 3 in 0.000ms
Finished first[1] on thread 0 in 0.000ms
Finished partition[1] on thread 1 in 0.000ms
```

```c
ac_schedule_t *ac_schedule_init(int argc, char **args, size_t num_partitions,
                                size_t cpus, size_t ram);
```

ac\_schedule\_init is initialized with the command line arguments, number of partitions, number of cpus, and MB of ram.  The command line arguments are passed to ac\_schedule\_init to potentially allow arguments to control how processing will be done.  

```c
#include "ac_schedule.h"

...

int main(int argc, char *argv[]) {
  ac_schedule_t *scheduler = ac_schedule_init(argc - 1, argv + 1, 2, 4, 10);
```

In the example, the command line arguments are passed and the scheduler is setup to have 2 partitions, 4 cpus, and 10 MB of RAM.

```c
ac_task_t *ac_schedule_task(ac_schedule_t *h, const char *task_name,
                            bool partitioned, ac_task_f setup);
```

Once the scheduler is initialized via ac\_schedule\_init, at least one task must be assigned to it.  ac\_schedule\_task schedules a task by naming it, defining if it is partitioned or not, and specifying a setup function to finish setting the task up.  In our example, we've assigned 5 tasks (the first one (named split) is not partitioned).

```c
ac_schedule_task(scheduler, "split", false, setup_task);
ac_schedule_task(scheduler, "partition", true, setup_task);
ac_schedule_task(scheduler, "first", true, setup_task);
ac_schedule_task(scheduler, "all", true, setup_task);
ac_schedule_task(scheduler, "multi", true, setup_task);
```

Normally, each scheduled task would have a different setup function.  In this case, all of the functions do the same thing (print the task name and the partition), so setup\_task can be shared.

```c
void ac_schedule_run(ac_schedule_t *h, ac_worker_f on_complete);
```

ac\_schedule\_run calls all of the setup methods specified via ac\_schedule\_task, sets up how all of the tasks will ultimately run, and finally runs them.  If the on\_complete call is not NULL, it will be called once a task completes.  ac\_worker\_complete is provided by ac\_schedule.h/c (it provides basic information).

```c
  ac_schedule_run(scheduler, ac_worker_complete);

  ac_schedule_destroy(scheduler);
  return 0;
}
```

The scheduler is run and then destroyed.

```c
bool do_nothing(ac_worker_t *w) { return true; }

bool setup_task(ac_task_t *task) {
  ac_task_runner(task, do_nothing);
  return true;
}
```

The setup\_task method is used to setup all 5 of the tasks.  Typically, there would be one setup function for each task.  This sets the method to run each partition of the given task.  The do\_nothing function returns true to indicate that the function succeeded.

This is a very basic shell of a program.  Here is a quick recap...

In the main function
1. initialize the scheduler using ac\_schedule\_init
2. add tasks to the scheduler using ac\_schedule\_task (each task will have a name, be partitioned or not, and have a setup function to be called later)
3. run the scheduler
4. destroy the scheduler

Next,
1. Implement the setup routines to define each task
2. Implement the routines that are referenced in the setup routines

Now, let's consider the output from before.
```
$ make start
$ ./start
Finished multi[0] on thread 0 in 0.000ms
Finished first[0] on thread 2 in 0.000ms
Finished all[0] on thread 1 in 0.000ms
Finished partition[0] on thread 3 in 0.000ms
Finished split[0] on thread 0 in 0.000ms
Finished multi[1] on thread 2 in 0.000ms
Finished all[1] on thread 3 in 0.000ms
Finished first[1] on thread 0 in 0.000ms
Finished partition[1] on thread 1 in 0.000ms
```

All of the tasks finish in 0.000ms which is expected because the tasks do nothing except return true.

Notice that all of the tasks are run on threads.  There is a thread for each cpu specified in ac\_schedule\_init.  There are 5 tasks with [0] and 4 tasks with [1].  The split[0] doesn't have a corresponding split[1].  This is because split was defined as not being partitioned.

Run start again...
```
$ ./start
$
```

Nothing was output.  This is because ac\_schedule will assume that the tasks don't need to rerun as there isn't anything to indicate that the tasks inputs have changed. Let's rerun again with a -h option.

```
$ ./start -h
The scheduler is meant to aid in running tasks in parallel.
At the moment, it operates on a single host - but I'm planning
on improving it to support multiple computers.


-f|--force rerun selected tasks even if they don't need run

-t <task[:partitions]>[<task[:partitions]], select tasks and
   optionally partitions.  tasks are separated by vertical bars
   (|) and partitions are sub-selected by placing a colon and then
   the partitions.  The partitions can be a single partition
   number, arange separated by a - (1-3), or a list of single
   partitions or ranges separated by commas.  To select partitions
   1, 3, 4, and 5 of task named first_task
   -t first_task:1,3-5

-o  Normally, all tasks that are needed to run to complete
    selected task will run.  This will override that behavior
    and only run selected tasks

-d|--dump <filename1,[filename2],...> dump the contents of files

-p|--prefix <filename1,[filename2],...> dump the contents of files
    and prefix each line with the line number

-l|--list list details of execution (the plan)

-s|--show-files similar to list, except input and output files
     are also displayed.

-c|--cpus <num_cpus> overrides default number of cpus

-r|--ram <ram MB> overrides default ram usage

-h|--help show this help message
```

The first option is to force tasks to rerun even if they don't need to.

```
$ ./start -f
Finished multi[0] on thread 1 in 0.000ms
Finished first[0] on thread 3 in 0.000ms
Finished all[0] on thread 0 in 0.001ms
Finished partition[0] on thread 2 in 0.000ms
Finished split[0] on thread 1 in 0.000ms
Finished all[1] on thread 0 in 0.000ms
Finished first[1] on thread 2 in 0.000ms
Finished partition[1] on thread 1 in 0.000ms
Finished multi[1] on thread 3 in 0.001ms
```

The next option allows for task subselection.  Maybe we only want to rerun partition 1 of first.
```
$ ./start -f -t first:1
Finished first[1] on thread 1 in 0.000ms
```

Or both partitions of first and the last multi partition.
```
$ ./start -f -t first:0-1 multi:1
Finished first[0] on thread 1 in 0.000ms
Finished first[1] on thread 2 in 0.000ms
Finished multi[1] on thread 0 in 0.000ms
```

If selecting all partitions of a task, this also works
```
$ ./start -f -t first
Finished first[0] on thread 1 in 0.000ms
Finished first[1] on thread 2 in 0.000ms
```

By default the scheduler will run all of the tasks if they haven't been run.  For example, if you were to remove the tasks folder and then try and only run first, the following would happen.

```
$ rm -rf tasks/
$ ./start -t first
Finished multi[0] on thread 0 in 0.000ms
Finished partition[0] on thread 2 in 0.000ms
Finished all[0] on thread 1 in 0.000ms
Finished first[0] on thread 3 in 0.000ms
Finished split[0] on thread 0 in 0.000ms
Finished multi[1] on thread 2 in 0.000ms
Finished first[1] on thread 3 in 0.000ms
Finished partition[1] on thread 0 in 0.000ms
Finished all[1] on thread 1 in 0.000ms
```

If the -o option is used, it will only run tasks which are required to do the selected tasks and the selected tasks.

```
$ rm -rf tasks
$ ./start -t first -o
Finished first[0] on thread 0 in 0.000ms
Finished first[1] on thread 1 in 0.000ms
```

The -l option lists the tasks and information about the tasks.  At this point, there is very little information about each task.  The tasks have a name, partition information, and a custom runner.  The -s option is similar to -l, except it will show more information (if available).

```
$ ./start -l
task: multi [0/2]
  custom runner
task: all [0/2]
  custom runner
task: first [0/2]
  custom runner
task: partition [0/2]
  custom runner
task: split [0/1]
  custom runner
task: multi [1/2]
  custom runner
task: all [1/2]
  custom runner
task: first [1/2]
  custom runner
task: partition [1/2]
  custom runner
```

The -d, -s, and -p options will make sense in a bit.

The -c option allows the number of cpus to be controlled.
```
$ ./start -f -c 1
Finished multi[0] on thread 0 in 0.000ms
Finished all[0] on thread 0 in 0.000ms
Finished first[0] on thread 0 in 0.000ms
Finished partition[0] on thread 0 in 0.000ms
Finished split[0] on thread 0 in 0.000ms
Finished multi[1] on thread 0 in 0.000ms
Finished all[1] on thread 0 in 0.000ms
Finished first[1] on thread 0 in 0.001ms
Finished partition[1] on thread 0 in 0.000ms
```

Notice that all threads run on once cpu.  

The -r option is similar to the -c option, except that it controls how much ram can be used.

In the above example, it may have been desirable for a given task to run everytime and not have to use the -f option to run task over again.  ac\_task\_run\_evertime is meant to be called from the setup function to do just that.

```c
bool setup_task(ac_task_t *task) {
  ac_task_run_everytime(task);
  ac_task_runner(task, do_nothing);
  return true;
}
```

## Ordering the tasks

In the last section, the tasks ran in a somewhat random order.  This would be okay if the tasks were in no way dependent upon each other, but that's rarely the case.  Let's consider the code that assigns tasks to the scheduler.

```c
ac_schedule_task(scheduler, "split", false, setup_task);
ac_schedule_task(scheduler, "partition", true, setup_task);
ac_schedule_task(scheduler, "first", true, setup_task);
ac_schedule_task(scheduler, "all", true, setup_task);
ac_schedule_task(scheduler, "multi", true, setup_task);
```

It may be desirable to have each task depend upon the previous task, except multi which will depend upon all of the tasks.  In order to do this, a setup function will have to be created for each task.

```c
ac_schedule_task(scheduler, "split", false, setup_split);
ac_schedule_task(scheduler, "partition", true, setup_partition);
ac_schedule_task(scheduler, "first", true, setup_first);
ac_schedule_task(scheduler, "all", true, setup_all);
ac_schedule_task(scheduler, "multi", true, setup_multi);
```

Next, each of those functions will have to be created.

```c
bool setup_split(ac_task_t *task) {
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_first(ac_task_t *task) {
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_partition(ac_task_t *task) {
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_all(ac_task_t *task) {
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_multi(ac_task_t *task) {
  ac_task_runner(task, do_nothing);
  return true;
}
```

For this example, we can continue with the do\_nothing which just returns true.

```c
bool ac_task_dependency(ac_task_t *task, const char *dependency);
bool ac_task_partial_dependency(ac_task_t *task, const char *dependency);
```

ac\_task\_dependency creates a full dependency upon listed tasks (const char *dependency is a vertical bar separated list of tasks).  ac\_task\_partial\_dependency creates a dependency upon the previous task and the given partition (unless the previous task isn't partitioned, then it is the same as ac\_task\_dependency).

The following will wire up the dependencies.  


```c
bool setup_split(ac_task_t *task) {
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_first(ac_task_t *task) {
  ac_task_dependency(task, "split");
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
  ac_task_dependency(task, "all|partition|first|split");
  ac_task_runner(task, do_nothing);
  return true;
}
```

The code for this is found in examples/mapreduce2/order\_tasks.c.  Deleting the tasks directory will cleanup all previous run information (from the last section for example).

```
$ rm -rf tasks
$ make order_tasks
$ ./order_tasks
Finished split[0] on thread 2 in 0.000ms
Finished first[0] on thread 2 in 0.001ms
Finished first[1] on thread 1 in 0.000ms
Finished partition[0] on thread 1 in 0.000ms
Finished all[0] on thread 1 in 0.000ms
Finished partition[1] on thread 2 in 0.000ms
Finished all[1] on thread 2 in 0.000ms
Finished multi[0] on thread 2 in 0.000ms
Finished multi[1] on thread 3 in 0.000ms
```

Notice how all of the tasks are in order.  all and partition are ordered on a per partition basis.  all[0] runs after partition[0] and all[1] runs after partition[1].

In the last section the -l option showed very little information.  Rerunning it now, will show more detail.

```
$ ./order_tasks -l
task: split [0/1]
  reverse dependencies:  multi[2] first[2]
  custom runner
task: first [0/2]
  dependencies:  split[1]
  reverse dependencies:  multi[2] partition[2]
  custom runner
task: first [1/2]
  dependencies:  split[1]
  reverse dependencies:  multi[2] partition[2]
  custom runner
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
  dependencies:  split[1] first[2] partition[2] all[2]
  custom runner
task: multi [1/2]
  dependencies:  split[1] first[2] partition[2] all[2]
  custom runner
```

This shows that each multi partition depends upon split, first, partition, and all.  It shows that all has a reverse dependency of multi (multi depends upon all).  all also has a partial dependency of partition.  The scheduler makes sure that the dependencies are complete before a given task/partition can be run.

examples/mapreduce2/order\_tasks.c
```c
#include "ac_schedule.h"

bool do_nothing(ac_worker_t *w) { return true; }

bool setup_split(ac_task_t *task) {
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_first(ac_task_t *task) {
  ac_task_dependency(task, "split");
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
  ac_task_dependency(task, "all|partition|first|split");
  ac_task_runner(task, do_nothing);
  return true;
}

int main(int argc, char *argv[]) {
  ac_schedule_t *scheduler = ac_schedule_init(argc - 1, argv + 1, 2, 4, 10);

  ac_schedule_task(scheduler, "split", false, setup_split);
  ac_schedule_task(scheduler, "partition", true, setup_partition);
  ac_schedule_task(scheduler, "first", true, setup_first);
  ac_schedule_task(scheduler, "all", true, setup_all);
  ac_schedule_task(scheduler, "multi", true, setup_multi);

  ac_schedule_run(scheduler, ac_worker_complete);

  ac_schedule_destroy(scheduler);
  return 0;
}
```

## Incorporating input files

Each task within the scheduler will typically have inputs and outputs and in general those inputs and outputs will be files.  Within a larger overall set of tasks, there will be inputs which originate outside the scheduler (content that has been produced from other systems).  Those should be the only inputs that need defined.  All other inputs should be artifacts and/or outputs from other tasks.

In the completely contrived example from the last section, the task named "split" originates every other job.  This section will focus on how to get the input into the scheduler.

The first change is to add a line to setup\_split indicating that their is outside input using ac\_task\_input\_files.  ac\_task\_input\_files expects the task, an arbitrary name for the input, a percentage (0.0-1.0) of RAM that can be used for the given input, and a function to call to get the list of input files.

```c
void ac_task_input_files(ac_task_t *task, const char *name, double ram_pct,
                         ac_worker_file_info_f file_info);
```

The callback signature
```c
ac_io_file_info_t *file_info(ac_worker_t *w, size_t *num_files,
                             ac_worker_input_t *inp);
```

Within the scheduler, there is a scheduler, tasks, and workers.  A task is a job to do that may be partitioned.  Workers are a particular partition of a task.  The setup routine sets up how workers can accomplish their goal.

```c
bool setup_split(ac_task_t *task) {
  ac_task_input_files(task, "split_input", 0.1, get_input_files);
  ac_task_runner(task, do_nothing);
  return true;
}
```

The ac\_task\_input\_files function is called within setup\_split.  This should be called prior to ac\_task\_runner.  get\_input\_files is the second change.  If split were multiple partitions (it is not in this case), each call to get\_input\_files should return the set of input files which relate to the given partition.

```c
bool file_ok(const char *filename, void *arg) {
  char **extensions = (char **)arg;
  char **p = extensions;
  while (*p) {
    if (ac_io_extension(filename, *p))
      return true;
    p++;
  }
  return false;
}

ac_io_file_info_t *get_input_files(ac_worker_t *w, size_t *num_files,
                                   ac_worker_input_t *inp) {
  *num_files = 0;
  char **extensions = ac_pool_split(w->worker_pool, NULL, ',', "tbontb");
  size_t num_inputs = 0;
  ac_io_file_info_t *inputs = ac_pool_io_list(w->worker_pool, "sample",
                                              &num_inputs, file_ok, extensions);
  *num_files = num_inputs;
  return inputs;
}
```

In [Listing the Files](https://www.anotherclibrary.com/docs/ac-mapreduce#listing-the-files) from the first post about map/reduce there is an explanation for how to use the AC library to list files (file\_ok and ac\_io\_list is explained).  The worker (w) has two pools that can be used.  Memory allocated from the worker\_pool will remain valid for the entire duration of the worker (processing of a particular partition of a task).  pool is the second member of ac\_worker\_t and is meant to be cleared frequently (after every record or group of records).  This function is run before the runner (do\_nothing at the moment), so it should use worker\_pool.  In Listing the Files, ac\_split and ac\_io\_list were used.  ac\_pool\_split and ac\_pool\_io\_list are alternatives that use the pool.  Because the pool is used, there is no need to later free the memory.

```
$ make input_data_1
$ ./input_data_1 -s -t split
task: split [0/1]
  reverse dependencies:  multi[2] first[2]
  custom runner
      input[0]: split_input (1)
        sample/sample.tbontb (20)
```

This task still doesn't do anything, but by using the -s option (and -t split to only show split task), you can see that the sample/sample.tbontb file is considered input.  The code is not particularly useful since the input files are hard coded into get\_input\_files.  That will be remedied in the next section.

```
$ rm -rf tasks
$ ./input_data_1
Finished split[0] on thread 1 in 0.000ms
Finished first[1] on thread 2 in 0.000ms
Finished first[0] on thread 0 in 0.000ms
Finished partition[0] on thread 0 in 0.000ms
Finished partition[1] on thread 3 in 0.000ms
Finished all[0] on thread 0 in 0.000ms
Finished all[1] on thread 3 in 0.000ms
Finished multi[0] on thread 3 in 0.000ms
Finished multi[1] on thread 0 in 0.000ms
$ ./input_data_1
$
```

Running input\_data\_1 the second time doesn't do anything because the scheduler doesn't see anything to do.  However, if you touch sample/sample.tbontb, everything will be rerun because split is the source of every other task.

```
$ touch sample/sample.tbontb
$ ./input_data_1
Finished split[0] on thread 1 in 0.000ms
Finished first[1] on thread 2 in 0.000ms
Finished first[0] on thread 0 in 0.000ms
Finished partition[0] on thread 0 in 0.000ms
Finished partition[1] on thread 3 in 0.000ms
Finished all[0] on thread 0 in 0.000ms
Finished all[1] on thread 3 in 0.000ms
Finished multi[0] on thread 3 in 0.000ms
Finished multi[1] on thread 0 in 0.000ms
$
```

examples/mapreduce2/input\_data\_1.c
```c
#include "ac_schedule.h"

bool file_ok(const char *filename, void *arg) {
  char **p = (char **)arg;
  while (*p) {
    if (ac_io_extension(filename, *p))
      return true;
    p++;
  }
  return false;
}

ac_io_file_info_t *get_input_files(ac_worker_t *w, size_t *num_files,
                                   ac_worker_input_t *inp) {
  *num_files = 0;
  char **extensions = ac_pool_split(w->worker_pool, NULL, ',', "tbontb");
  size_t num_inputs = 0;
  ac_io_file_info_t *inputs = ac_pool_io_list(w->worker_pool, "sample",
                                              &num_inputs, file_ok, extensions);
  *num_files = num_inputs;
  return inputs;
}

bool do_nothing(ac_worker_t *w) { return true; }

bool setup_split(ac_task_t *task) {
  ac_task_input_files(task, "split_input", 0.1, get_input_files);
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_first(ac_task_t *task) {
  ac_task_dependency(task, "split");
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
  ac_task_dependency(task, "all|partition|first|split");
  ac_task_runner(task, do_nothing);
  return true;
}

int main(int argc, char *argv[]) {
  ac_schedule_t *scheduler = ac_schedule_init(argc - 1, argv + 1, 2, 4, 10);

  ac_schedule_task(scheduler, "split", false, setup_split);
  ac_schedule_task(scheduler, "partition", true, setup_partition);
  ac_schedule_task(scheduler, "first", true, setup_first);
  ac_schedule_task(scheduler, "all", true, setup_all);
  ac_schedule_task(scheduler, "multi", true, setup_multi);

  ac_schedule_run(scheduler, ac_worker_complete);

  ac_schedule_destroy(scheduler);
  return 0;
}
```


## Command Line Arguments

In the last example, the directory and the file extension were hard coded for the input files.  In this section, I'll show how to use command line arguments and configuration.  Adding arguments involves a fair amount of code, but it's pretty straight forward.


In the main function, ac\_schedule\_custom\_args specifies a custom usage, a custom argument parser, a command that runs once all arguments have been processed.  

```
typedef struct {
  size_t num_inputs;
  ac_io_file_info_t *inputs;
  char *dir;
  char *ext;
  char **extensions;
} custom_arg_t;

...

int main(int argc, char *argv[]) {
  ac_schedule_t *scheduler = ac_schedule_init(argc - 1, argv + 1, 2, 4, 10);
  custom_arg_t custom;
  memset(&custom, 0, sizeof(custom));

  ac_schedule_custom_args(scheduler, custom_usage, parse_custom_args,
                          finish_custom_args, &custom);
  ...
  if (custom.inputs)
    ac_free(custom.inputs);
  if (custom.extensions)
    ac_free(custom.extensions);
```

The custom\_usage function just uses printf.
```c
void custom_usage() {
  printf("Select files within a given directory and do nothing!\n");
  printf("--dir <dir> - directory to scan\n");
  printf("--ext <extensions> - comma delimited list of file extensions to "
         "consider");
}
```

parse\_custom\_args returns 0 if there is a problem or the number of arguments to skip.  In our example, we are using argument pairs and returning 2 if first arg is --dir or --ext.
```c
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
```

finish\_custom\_args is called after all of the args are parsed.
```c
bool finish_custom_args(int argc, char **argv, void *arg) {
  custom_arg_t *ca = (custom_arg_t *)arg;
  if (!ca->dir)
    ca->dir = (char *)"sample";
  if (!ca->ext)
    ca->ext = (char *)"tbontb";

  ca->extensions = ac_split2(NULL, ',', ca->ext);
  ca->inputs = ac_io_list(ca->dir, &ca->num_inputs, file_ok, ca->extensions);
  if (ca->inputs)
    return true;
  return false;
}
```

Finally, ac\_task\_custom\_arg will retrieve the custom\_arg\_t structure in any function which passes an ac\_worker\_t.
```c
ac_io_file_info_t *get_input_files(ac_worker_t *w, size_t *num_files,
                                   ac_worker_input_t *inp) {
  custom_arg_t *ca = (custom_arg_t *)ac_task_custom_arg(w->task);
  *num_files = ca->num_inputs;
  return ca->inputs;
}
```

One issue with the command line args is that they need to be the same every time.  To facilitate this, the scheduler will save the arguments and requires the parameter --new-args if they change.  The custom\_arg\_t will remain in memory for the life of the scheduler and is expected to be able to be accessed in a thread-safe manner.

```
$ make input_data_2
$ rm -rf tasks
$ ./input_data_2 --dir .. --ext c,txt
Finished split[0] on thread 2 in 0.000ms
Finished first[0] on thread 2 in 0.000ms
Finished first[1] on thread 1 in 0.000ms
Finished partition[0] on thread 1 in 0.001ms
Finished partition[1] on thread 0 in 0.000ms
Finished all[0] on thread 1 in 0.000ms
Finished all[1] on thread 0 in 0.000ms
Finished multi[0] on thread 0 in 0.000ms
Finished multi[1] on thread 3 in 0.000ms
$ ./input_data_2 -s -t split
task: split [0/1]
  reverse dependencies:  multi[2] first[2]
  custom runner
      input[0]: split_input (74)
        ../ac_json/ac_json_dump_error_to_buffer.c (726)
        ../ac_json/ac_json_parse.c (1099)
        ../ac_json/ac_json_dump_error.c (627)
        ../ac_buffer/ac_buffer_sets.c (275)
        ../ac_buffer/ac_buffer_append.c (325)
        ../ac_buffer/ac_buffer_destroy.c (287)
        ../ac_buffer/ac_buffer_appends.c (296)
        ../ac_buffer/ac_buffer_clear.c (488)
$ ./input_data_2 --dir .. --ext md
[ERROR] Command line arguments don't match (c,txt != md) - (use --new-args?)

Find all words ending in .h, .c, and .md and sort by
frequency descending.

--dir <dir> - directory to scan
--ext <extensions> - comma delimited list of file extensions to consider
----------------------------------------------------------

The scheduler is meant to aid in running tasks in parallel.
```

In general, it would be undesirable for parameters to change in a job without first cleaning up the overall set of tasks.  This error is shown to prevent data corruption and to warn the user that they might be making a mistake.  The --new-args will override the error and continue.

examples/mapreduce2/input\_data\_2.c
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
  *num_files = ca->num_inputs;
  return ca->inputs;
}

bool do_nothing(ac_worker_t *w) { return true; }

bool setup_split(ac_task_t *task) {
  ac_task_input_files(task, "split_input", 0.1, get_input_files);
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_first(ac_task_t *task) {
  ac_task_dependency(task, "split");
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
  ac_task_dependency(task, "all|partition|first|split");
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

  ac_schedule_task(scheduler, "split", false, setup_split);
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

## Splitting up the input jobs

In the previous section, the input was setup to use command line arguments for the split task.  The split task is configured to not be partitioned.  In this section, the split task will be changed to a partitioned task.  In the main file, it is a relatively small change.

```c
ac_schedule_task(scheduler, "split", false, setup_split);
```

changes to
```c
ac_schedule_task(scheduler, "split", true, setup_split);
```

This alone would be enough if it was okay for every partition to parse all of the input files.  However, in our case, we will want to change the program to have each partition process some of the input files.  This could be done using every Nth file or by using a hash.  The advantage of using a hash over every Nth file is that if a file is added or removed to the file system, files may shift from one partition to another.  This may cause some partitions to not rerun as they should.  For our example, we will use a hash of the filename.  The ac\_io\_file\_info\_t structure has a member hash which is the hash of the filename.

The get\_input\_files needs to change to include only the appropriate files.
```c
ac_io_file_info_t *get_input_files(ac_worker_t *w, size_t *num_files,
                                   ac_worker_input_t *inp) {
  custom_arg_t *ca = (custom_arg_t *)ac_task_custom_arg(w->task);
  *num_files = ca->num_inputs;
  return ca->inputs;
}
```

changes to
```c
ac_io_file_info_t *get_input_files(ac_worker_t *w, size_t *num_files,
                                   ac_worker_input_t *inp) {
  custom_arg_t *ca = (custom_arg_t *)ac_task_custom_arg(w->task);
  ac_io_file_info_t *p = ca->inputs;
  ac_io_file_info_t *ep = p + ca->num_inputs;
  size_t num_matching = 0;
  while(p < ep) {
    if((p->hash % w->num_partitions) == p->partition)
      num_matching++;
    p++;
  }
  *num_files = num_matching;
  if(!num_matching)
    return NULL;
  p = ca->inputs;
  ac_io_file_info_t *res = (ac_io_file_info_t *)ac_pool_alloc(w->worker_pool, sizeof(ac_io_file_info_t) * num_matching);
  ac_io_file_info_t *wp = res;
  while(p < ep) {
    if((p->hash % w->num_partitions) == p->partition) {
      *wp = *p;
      wp++;
    }
    p++;
  }
  return res;
}
```

This will count the number of inputs that belong to this partition, allocate an array using the pool that persists for the duration of the task, fill the array, and return it.  There is a function to select inputs in ac\_io named ac\_io\_select\_file\_info which does most of what the above function does.


```c
ac_io_file_info_t *ac_io_select_file_info(ac_pool_t *pool, size_t *num_res,
                                          ac_io_file_info_t *inputs,
                                          size_t num_inputs, size_t partition,
                                          size_t num_partitions);
```

Using that function, the new get\_input\_files...
```c
ac_io_file_info_t *get_input_files(ac_worker_t *w, size_t *num_files,
                                   ac_worker_input_t *inp) {
  custom_arg_t *ca = (custom_arg_t *)ac_task_custom_arg(w->task);
  return ac_io_select_file_info(w->worker_pool, num_files,
                                ca->inputs, ca->num_inputs,
                                w->partition, w->num_partitions);
}
```

```
$ rm -rf tasks
$ ./input_data_3 --dir .. --ext c
Finished split[0] on thread 0 in 0.000ms
Finished split[1] on thread 2 in 0.000ms
Finished first[0] on thread 2 in 0.000ms
Finished first[1] on thread 2 in 0.000ms
Finished partition[0] on thread 2 in 0.000ms
Finished all[0] on thread 2 in 0.000ms
Finished partition[1] on thread 1 in 0.000ms
Finished all[1] on thread 1 in 0.000ms
Finished multi[0] on thread 1 in 0.000ms
Finished multi[1] on thread 3 in 0.000ms
$ ./input_data_3 -s -t split
task: split [0/2]
  reverse dependencies:  multi[2] first[2]
  custom runner
      input[0]: split_input (25)
        ../ac_buffer/ac_buffer_append.c (325)
        ../ac_buffer/ac_buffer_appends.c (296)
        ../ac_buffer/ac_buffer_clear.c (488)
        ../ac_buffer/ac_buffer_init.c (287)
        ...        
task: split [1/2]
  reverse dependencies:  multi[2] first[2]
  custom runner
      input[0]: split_input (47)
        ../ac_json/ac_json_dump_error_to_buffer.c (726)
        ../ac_json/ac_json_parse.c (1099)
        ../ac_json/ac_json_dump_error.c (627)
        ../ac_buffer/ac_buffer_sets.c (275)
        ...
```

examples/mapreduce2/input\_data\_3.c
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
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_first(ac_task_t *task) {
  ac_task_dependency(task, "split");
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
  ac_task_dependency(task, "all|partition|first|split");
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

## Ack files

In the last section, input files were wired up to the split task.  We saw how touching the input files would cause split to run over again.  Inside the tasks folder that is created by scheduler, there is an ack folder.  After each partition is run, there will be a file inside the ack folder.  If the ack file for a particular task / partition is touched, the task / partition will run again and all tasks that depend upon that task / partition.

```
$ ls tasks
ack		all_0		all_1		first_0		first_1		multi_0		multi_1		partition_0	partition_1	split_0
$ ls tasks/ack
all_0		all_1		first_0		first_1		multi_0		multi_1		partition_0	partition_1	split_0
```

Consider the partition:0 task/partition..
```
$ ./input_data_3 -l -t partition:0
task: partition [0/2]
  dependencies:  first[2]
  reverse dependencies:  multi[2]
  reverse partial dependencies:  all[2]
  custom runner
```

There is a full reverse dependency of multi and a reverse partitioned dependency of all.  If tasks/ack/partition\_0 is removed, it should cause partition[0], all[0], multi[0], and multi[1] to run.

```
$ rm tasks/ack/partition_0
$ ./input_data_3
Finished partition[0] on thread 1 in 0.000ms
Finished all[0] on thread 1 in 0.000ms
Finished multi[0] on thread 1 in 0.000ms
Finished multi[1] on thread 2 in 0.000ms
```

If instead of removing tasks/ack/partition\_0, it is touched, then all[0], multi[0], and multi[1] will be run.  This ack file is touched after a process finishes, so touching the ack file is equivalent to the task/partition finishing at the time the ack file is touched.

```
$ touch tasks/ack/partition_0
$ ./input_data_3
Finished all[0] on thread 0 in 0.000ms
Finished multi[0] on thread 0 in 0.000ms
Finished multi[1] on thread 1 in 0.000ms
```

## Dumping the input files

The input files for the split task partition 0 can be shown as follows..
```
$ ./input_data_3 -s -t split:0
task: split [0/2]
  reverse dependencies:  multi[2] first[2]
  custom runner
      input[0]: split_input (28)
        ../ac_buffer/ac_buffer_append.c (325)
        ../ac_buffer/ac_buffer_appends.c (296)
        ../ac_buffer/ac_buffer_clear.c (488)
        ../ac_buffer/ac_buffer_init.c (287)
        ../ac_buffer/ac_buffer_shrink_by.c (299)
        ../ac_buffer/ac_buffer_appendn.c (583)
        ../ac_buffer/ac_buffer_data.c (287)
        ../ac_buffer/ac_buffer_setn.c (266)
```

If you run ./input\_data\_3 -h, the help shows the following two options..
```
$ ./input_data_3 -h

   ...

-d|--dump <filename1,[filename2],...> dump the contents of files

-p|--prefix <filename1,[filename2],...> dump the contents of files
    and prefix each line with the line number

   ...
```

We can try to dump one of the input files...
```
$ ./input_data_3 -d ../ac_buffer/ac_buffer_setn.c
$
```

and nothing happens.  This is because the scheduler doesn't know how to dump the input files.  In ac\_schedule.h/c there is a function which dumps text and it is defined as.

```c
void ac_task_dump_text(ac_worker_t *w, ac_io_record_t *r, ac_buffer_t *bh,
                       void *arg) {
  ac_buffer_appends(bh, r->record);
}
```

ac\_task\_dump\_text is a very simple function which simply appends the contents of the record to the buffer.  If the format of input file is not plain text, then you will need to implement your own dump function which should dump text to the buffer.

```
bool setup_split(ac_task_t *task) {
  ac_task_input_files(task, "split_input", 0.1, get_input_files);
  ac_task_input_dump(task, ac_task_dump_text, NULL);
  ac_task_runner(task, do_nothing);
  return true;
}
```

The ac\_task\_dump\_text must be called after the associated ac\_task\_input\_files call and not before another one or an ac\_task\_output call (described later).

```
$ make input_data_4
$ ./input_data_4 -d ../ac_buffer/ac_buffer_setn.c
$
```

This still isn't enough.  The input record delimiter must be specified.  The format choices are delimited (records end in a given character such as a newline or a zero), fixed (the records are determined by a fixed length), and prefix (the records are determined by a 4 byte length prefix before each record).  The text files are newline delimited, so this must be specified (again after ac\_task\_input\_files).

```
bool setup_split(ac_task_t *task) {
  ac_task_input_files(task, "split_input", 0.1, get_input_files);
  ac_task_input_format(task, ac_io_delimiter('\n'));
  ac_task_input_dump(task, ac_task_dump_text, NULL);
  ac_task_runner(task, do_nothing);
  return true;
}
```

Trying again...
```
$ make input_data_4
$ ./input_data_4 -d  ../ac_buffer/ac_buffer_setn.c
#include "ac_buffer.h"
int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setn(bh, 'H', 20);
  /* print HHHHHHHHHHHHHHHHHHHH followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
```

And it works!

The full source code is found in examples/mapreduce2/input\_data\_4.c.  I've ommitted the code as it only adds the following two lines to setup\_split.
```c
ac_task_input_format(task, ac_io_delimiter('\n'));
ac_task_input_dump(task, ac_task_dump_text, NULL);
```

The next part will discuss how to order tasks using a data pipeline.
