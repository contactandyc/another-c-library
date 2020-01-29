---
path: "/ac-mapreduce2"
posttype: "docs"
title: "AC's Map Reduce Framework Part 2"
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

ac_schedule_init is initialized with the command line arguments, number of partitions, number of cpus, and MB of ram.  The command line arguments are passed to ac_schedule_init to potentially allow arguments to control how processing will be done.  

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

Once the scheduler is initialized via ac_schedule_init, at least one task must be assigned to it.  ac_schedule_task schedules a task by naming it, defining if it is partitioned or not, and specifying a setup function to finish setting the task up.  In our example, we've assigned 5 tasks (the first one (named split) is not partitioned).

```c
ac_schedule_task(scheduler, "split", false, setup_task);
ac_schedule_task(scheduler, "partition", true, setup_task);
ac_schedule_task(scheduler, "first", true, setup_task);
ac_schedule_task(scheduler, "all", true, setup_task);
ac_schedule_task(scheduler, "multi", true, setup_task);
```

Normally, each scheduled task would have a different setup function.  In this case, all of the functions do the same thing (print the task name and the partition), so setup_task can be shared.

```c
void ac_schedule_run(ac_schedule_t *h, ac_worker_f on_complete);
```

ac_schedule_run calls all of the setup methods specified via ac_schedule_task, sets up how all of the tasks will ultimately run, and finally runs them.  If the on_complete call is not NULL, it will be called once a task completes.  ac_worker_complete is provided by ac_schedule.h/c (it provides basic information).

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

The setup_task method is used to setup all 5 of the tasks.  Typically, there would be one setup function for each task.  This sets the method to run each partition of the given task.  The do_nothing function returns true to indicate that the function succeeded.

This is a very basic shell of a program.  Here is a quick recap...

In the main function
1. initialize the scheduler using ac_schedule_init
2. add tasks to the scheduler using ac_schedule_task (each task will have a name, be partitioned or not, and have a setup function to be called later)
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

Notice that all of the tasks are run on threads.  There is a thread for each cpu specified in ac_schedule_init.  There are 5 tasks with [0] and 4 tasks with [1].  The split[0] doesn't have a corresponding split[1].  This is because split was defined as not being partitioned.

Run start again...
```
$ ./start
$
```

Nothing was output.  This is because ac_schedule will assume that the tasks don't need to rerun as there isn't anything to indicate that the tasks inputs have changed. Let's rerun again with a -h option.

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

By default the AC's scheduler will run all of the tasks if they haven't been run.  For example, if you were to remove the tasks folder and then try and only run first, the following would happen.

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

In the above example, it may have been desirable for a given task to run everytime and not have to use the -f option to run task over again.  ac_task_run_evertime is meant to be called from the setup function to do just that.

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

For this example, we can continue with the do_nothing which just returns true.

```c
bool ac_task_dependency(ac_task_t *task, const char *dependency);
bool ac_task_partial_dependency(ac_task_t *task, const char *dependency);
```

ac_task_dependency creates a full dependency upon listed tasks (const char *dependency is a vertical bar separated list of tasks).  ac_task_partial_dependency creates a dependency upon the previous task and the given partition (unless the previous task isn't partitioned, then it is the same as ac_task_dependency).

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

The code for this is found in examples/mapreduce2/order_tasks.c.  Deleting the tasks directory will cleanup all previous run information (from the last section for example).

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
