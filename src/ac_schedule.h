#ifndef _ac_schedule_H
#define _ac_schedule_H

#include "ac_buffer.h"
#include "ac_common.h"
#include "ac_pool.h"

#include <time.h>

struct ac_schedule_s;
typedef struct ac_schedule_s ac_schedule_t;

struct ac_task_s;
typedef struct ac_task_s ac_task_t;

struct ac_task_part_s;
typedef struct ac_task_part_s ac_task_part_t;

struct ac_task_input_s;
typedef struct ac_task_input_s ac_task_input_t;

typedef bool (*ac_task_f)(ac_task_t *task);
typedef bool (*ac_task_part_f)(ac_task_part_t *tp);
typedef bool (*ac_task_check_input_f)(ac_task_part_t *tp, ac_task_input_t *inp,
                                      time_t ack_time);

/**************************************************************************
The following functions should be called to setup the scheduler.  Once all
of the tasks are scheduled, call ac_schedule_run.  This will cause the setup
functions to be called in the order that they were scheduled.  The scheduler
allows you to define a number of partitions, cpus, ram, and disk space.  It
then attempts to keep your process operating fully within that space.
***************************************************************************/

/* Create the scheduler and specify how many partitions, cpus, ram (in MB),
   and disk space.  Disk space is currently unused - but I have plans for it. */
ac_schedule_t *ac_schedule_init(size_t num_partitions, size_t cpus, size_t ram,
                                size_t disk_space);

/* Define where the ack directory should be (default is tasks/ack)*/
void ac_schedule_ack_dir(ac_schedule_t *h, const char *ack_dir);

/* Define where the tasks directory should be output to (default is tasks) */
void ac_schedule_task_dir(ac_schedule_t *h, const char *task_dir);

/* Add a task to the scheduler with an associated name and define whether it
   is partitioned or not.  The setup function will further describe the task
   once all of the tasks have been added to the scheduler. */
ac_task_t *ac_schedule_task(ac_schedule_t *h, const char *task_name,
                            bool partitioned, ac_task_f setup);

/* Run all of the tasks */
void ac_schedule_run(ac_schedule_t *h, ac_task_part_f on_complete);

/* Destroy the scheduler */
void ac_schedule_destroy(ac_schedule_t *h);

/**************************************************************************
The following functions should be called from the setup callback methods
specified by ac_schedule_task.
***************************************************************************/

/* Define what should run for the given task */
void ac_task_runner(ac_task_t *task, ac_task_part_f runner);

/* Define outside input files to a task.  Inputs from other tasks are auto
   configured through ac_task_output. Inputs are named for convenience with
   the expectation that the check method would confirm if data has changed.

   The ram_pct is what percentage of ram this input should use for the given
   task (and should range from 0-1).
   */
void ac_task_input_files(ac_task_t *task, const char *name, double ram_pct,
                         ac_task_check_input_f check);

/* Once intermediate files are no longer required, they are removed unless
   AC_OUTPUT_KEEP is defined. */
static const size_t AC_OUTPUT_KEEP = 1;

/* Files are typically assumed to not be split and meant to be used from
   one partition to the next.  For example, if partition 3 of a task produces
   an output to destination task, it is assumed that the output is meant for
   partition 3 of the destination task.

   AC_OUTPUT_USE_FIRST would cause the destination task to use the first
   partition of the source task.

   AC_OUTPUT_SPLIT would cause the destination task to use all of the source
   partitions data.
*/
static const size_t AC_OUTPUT_USE_FIRST = 2;
static const size_t AC_OUTPUT_SPLIT = 4;

/* Defines an output which will use name as the base name.  The destinations
   are a list of output tasks which can be NULL to specify none, or multiple if
   separated by vertical bars.
*/
void ac_task_output(ac_task_t *task, const char *name, const char *destinations,
                    double out_ram_pct, double in_ram_pct, size_t flags);

/* Use this if the dependency must finish completely prior to task running.
   Vertical bars can seperate dependencies.
*/
bool ac_task_dependency(ac_task_t *task, const char *dependency);

/* Use this if the task can run partition by partition if dependency is filled.
   Vertical bars can seperate dependencies.
 */
bool ac_task_partial_dependency(ac_task_t *task, const char *dependency);

/* this allows a task to do nothing and can be used as a checkpoint or
   dependency for other tasks */
void ac_task_do_nothing(ac_task_t *task);

/* this forces a task to run everytime no matter what */
void ac_task_run_everytime(ac_task_t *task);

/**************************************************************************
The following structures are primarily used within the runner
***************************************************************************/

struct ac_task_state_link_s;
typedef struct ac_task_state_link_s ac_task_state_link_t;

struct ac_task_output_s;
typedef struct ac_task_output_s ac_task_output_t;

struct ac_task_link_s;
typedef struct ac_task_link_s ac_task_link_t;

struct ac_task_link_s {
  ac_task_t *task;
  ac_task_link_t *next;
};

struct ac_task_part_s {
  ac_pool_t *pool;
  ac_buffer_t *bh;
  ac_task_t *task;
  size_t partition;
  size_t num_partitions;
  size_t running;
  size_t thread_id;
  time_t ack_time;
  ac_task_state_link_t *__link;
};

struct ac_task_input_s {
  char *name;
  ac_task_check_input_f check;
  double ram_pct;
  ac_task_t *task;
  ac_task_output_t *src;
  ac_task_input_t *next;
};

struct ac_task_output_s {
  char *name;
  ac_task_link_t *destinations;
  double ram_pct;
  size_t flags;
  size_t num_partitions;
  ac_task_t *task;
  ac_task_output_t *next;
};

/**************************************************************************
The following functions are meant to be used within the runner or the check
callback.
***************************************************************************/

/* Get nth output (defined in the setup callback) */
ac_task_output_t *ac_task_get_output(ac_task_part_t *tp, size_t n);

/* Get nth input (defined by ac_task_input_files and by ac_task_output) */
ac_task_input_t *ac_task_get_input(ac_task_part_t *tp, size_t n);

/* Return the task name given a task */
const char *ac_task_name(ac_task_t *task);

/* Get the scheduler given a task - shouldn't be needed much if at all */
ac_schedule_t *ac_task_schedule(ac_task_t *task);

/* Return an actual amount of ram given the task/partition and percentage */
size_t ac_task_part_ram(ac_task_part_t *tp, double pct);

/* Return the output base name based upon a task/partition and an output */
char *ac_task_output_base(ac_task_part_t *tp, ac_task_output_t *outp);

/* Same as above except adding a suffix.  Suffix is somewhat complicated in
   the case where filenames have a compression extension. */
char *ac_task_output_base2(ac_task_part_t *tp, ac_task_output_t *outp,
                           const char *suffix);

/* Returns NULL if no input from the given source partition.  Otherwise,
   this will return the name of the file that was output to this partition
   given the task/part, input, and source partition */
char *ac_task_input_name(ac_task_part_t *tp, ac_task_input_t *inp,
                         size_t partition);

/**************************************************************************
Other helper functions
**************************************************************************/

/* Debugging - print the internals */
void ac_schedule_print(ac_schedule_t *h);

#endif
