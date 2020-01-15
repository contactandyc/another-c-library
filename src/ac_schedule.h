#ifndef _ac_schedule_H
#define _ac_schedule_H

#include "ac_buffer.h"
#include "ac_common.h"
#include "ac_in.h"
#include "ac_io.h"
#include "ac_out.h"
#include "ac_pool.h"

#include <time.h>

struct ac_schedule_s;
typedef struct ac_schedule_s ac_schedule_t;

struct ac_task_s;
typedef struct ac_task_s ac_task_t;

struct ac_worker_s;
typedef struct ac_worker_s ac_worker_t;

struct ac_worker_input_s;
typedef struct ac_worker_input_s ac_worker_input_t;

typedef bool (*ac_task_f)(ac_task_t *task);
typedef bool (*ac_worker_f)(ac_worker_t *w);

typedef void *(*ac_worker_data_f)(ac_worker_t *w);
typedef void (*ac_destroy_worker_data_f)(ac_worker_t *w, void *d);

typedef void (*ac_runner_f)(ac_worker_t *w, ac_io_record_t *r, ac_out_t **out);
typedef void (*ac_group_runner_f)(ac_worker_t *w, ac_io_record_t *r,
                                  size_t num_r, ac_out_t **out);

typedef ac_io_file_info_t *(*ac_worker_file_info_f)(ac_worker_t *w,
                                                    size_t *num_files,
                                                    ac_worker_input_t *inp);

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
void ac_schedule_run(ac_schedule_t *h, ac_worker_f on_complete);

/* Destroy the scheduler */
void ac_schedule_destroy(ac_schedule_t *h);

/**************************************************************************
The following functions should be called from the setup callback methods
specified by ac_schedule_task.
***************************************************************************/

/* Define what should run for the given task */
void ac_task_runner(ac_task_t *task, ac_worker_f runner);

void ac_task_default_runner(ac_task_t *task);

void ac_task_transform(ac_task_t *task, const char *inp, const char *outp,
                       ac_runner_f runner);

void ac_task_group_transform(ac_task_t *task, const char *inp, const char *outp,
                             ac_group_runner_f runner, ac_io_compare_f compare);

void ac_task_group_compare_arg(ac_task_t *task, ac_worker_data_f create,
                               ac_destroy_worker_data_f destroy);

void ac_task_transform_data(ac_task_t *task, ac_worker_data_f create,
                            ac_destroy_worker_data_f destroy);

/* Predefined task runners */
bool ac_task_in_out(ac_worker_t *w);
bool ac_task_in_out2(ac_worker_t *w);
bool ac_task_in_out_custom(ac_worker_t *w, ac_in_out_f cb, void *arg);
bool ac_task_in_out_custom2(ac_worker_t *w, ac_in_out2_f cb, void *arg);

/* Define outside input files to a task.  Inputs from other tasks are auto
   configured through ac_task_output. Inputs are named for convenience with
   the expectation that the check method would confirm if data has changed.

   The ram_pct is what percentage of ram this input should use for the given
   task (and should range from 0-1).
   */
void ac_task_input_files(ac_task_t *task, const char *name, double ram_pct,
                         ac_worker_file_info_f file_info);

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
static const size_t AC_OUTPUT_CHAINED = 8;

/* Defines an output which will use name as the base name.  The destinations
   are a list of output tasks which can be NULL to specify none, or multiple if
   separated by vertical bars.
*/
void ac_task_output(ac_task_t *task, const char *name, const char *destinations,
                    double out_ram_pct, double in_ram_pct, size_t flags);

/* These ac_task_output_... methods must be called after ac_task_output and
   will apply to the previous ac_task_output call. */
void ac_task_output_partition(ac_task_t *task, ac_io_partition_f part,
                              void *arg);

void ac_task_output_compare(ac_task_t *task, ac_io_compare_f compare,
                            void *compare_tag);

void ac_task_output_intermediate_compare(ac_task_t *task,
                                         ac_io_compare_f compare,
                                         void *compare_tag);

void ac_task_output_keep_first(ac_task_t *task);

void ac_task_output_reducer(ac_task_t *task, ac_io_reducer_f reducer,
                            void *reducer_tag);

void ac_task_output_intermediate_reducer(ac_task_t *task,
                                         ac_io_reducer_f reducer,
                                         void *reducer_tag);

void ac_task_output_group_size(ac_task_t *task, size_t num_per_group,
                               size_t start);

void ac_task_output_use_extra_thread(ac_task_t *task);

void ac_task_output_dont_compress_tmp(ac_task_t *task);

void ac_task_output_sort_before_partitioning(ac_task_t *task);

void ac_task_output_sort_while_partitioning(ac_task_t *task);

void ac_task_output_num_sort_threads(ac_task_t *task, size_t num_sort_threads);

void ac_task_output_format(ac_task_t *task, ac_io_format_t format);

void ac_task_output_safe_mode(ac_task_t *task);

void ac_task_output_write_ack_file(ac_task_t *task);

void ac_task_output_gz(ac_task_t *task, int level);

void ac_task_output_lz4(ac_task_t *task, int level, ac_lz4_block_size_t size,
                        bool block_checksum, bool content_checksum);

/* The ac_task_input... methods apply to the previous ac_task_input_files or
   ac_task_output call.  If the previous ac_task_output call doesn't specify
   one or more destinations, the calls are silently ignored. */
void ac_task_input_format(ac_task_t *task, ac_io_format_t format);

void ac_task_input_compare(ac_task_t *task, ac_io_compare_f compare,
                           void *compare_tag);

void ac_task_input_keep_first(ac_task_t *task);

void ac_task_input_reducer(ac_task_t *task, ac_io_reducer_f reducer,
                           void *reducer_tag);

void ac_task_input_compressed_buffer_size(ac_task_t *task, size_t buffer_size);

void ac_task_input_limit(ac_task_t *task, size_t limit);

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

struct ac_worker_output_s;
typedef struct ac_worker_output_s ac_worker_output_t;

struct ac_task_link_s;
typedef struct ac_task_link_s ac_task_link_t;

struct ac_task_link_s {
  ac_task_t *task;
  ac_task_link_t *next;
};

struct ac_schedule_thread_s;
typedef struct ac_schedule_thread_s ac_schedule_thread_t;

struct ac_worker_s {
  /* The worker pool should never be cleared */
  ac_pool_t *worker_pool;
  /* The pool can be cleared by application */
  ac_pool_t *pool;
  ac_task_t *task;
  ac_worker_input_t *inputs;
  ac_worker_output_t *outputs;
  void *data;
  void *transform_data;
  size_t partition;
  size_t num_partitions;
  size_t running;
  size_t thread_id;
  time_t ack_time;
  ac_schedule_thread_t *schedule_thread;
  ac_task_state_link_t *__link;
};

struct ac_worker_input_s {
  size_t id;
  char *name;
  ac_worker_file_info_f file_info;
  ac_io_file_info_t *files;
  size_t num_files;
  double ram_pct;

  ac_in_options_t options;
  ac_io_compare_f compare;
  void *compare_arg;
  ac_io_reducer_f reducer;
  void *reducer_arg;
  size_t limit;

  ac_task_t *task;
  ac_worker_output_t *src;
  ac_worker_input_t *next;
};

struct ac_worker_output_s {
  size_t id;
  char *name;
  ac_task_link_t *destinations;
  double ram_pct;
  size_t flags;
  size_t num_partitions;

  ac_out_options_t options;
  ac_out_ext_options_t ext_options;

  ac_task_t *task;
  ac_worker_output_t *next;
};

/**************************************************************************
The following functions are meant to be used within the runner or the check
callback.
***************************************************************************/

/* Get nth output (defined in the setup callback) */
ac_worker_output_t *ac_worker_output(ac_worker_t *w, size_t n);

/* Form an ac_out_t from the specifications for the given task */
ac_out_t *ac_worker_out(ac_worker_t *w, size_t n);

/* Get nth input (defined by ac_task_input_files and by ac_task_output) */
ac_worker_input_t *ac_worker_input(ac_worker_t *w, size_t n);

/* Form an ac_in_t from the specifications for the given task */
ac_in_t *ac_worker_in(ac_worker_t *w, size_t n);

/* Return the task name given a task */
const char *ac_task_name(ac_task_t *task);

/* Get the scheduler given a task - shouldn't be needed much if at all */
ac_schedule_t *ac_task_schedule(ac_task_t *task);

/* Return an actual amount of ram given the task/partition and percentage */
size_t ac_worker_ram(ac_worker_t *w, double pct);

/* Return the output base name based upon a task/partition and an output */
char *ac_worker_output_base(ac_worker_t *w, ac_worker_output_t *outp);

/* Same as above except adding a suffix.  Suffix is somewhat complicated in
   the case where filenames have a compression extension. */
char *ac_worker_output_base2(ac_worker_t *w, ac_worker_output_t *outp,
                             const char *suffix);

/* Returns NULL if no input from the given source partition.  Otherwise,
   this will return the name of the file that was output to this partition
   given the task/part, input, and source partition */
char *ac_worker_input_name(ac_worker_t *w, ac_worker_input_t *inp,
                           size_t partition);

/**************************************************************************
Other helper functions
**************************************************************************/

/* Debugging - print the internals */
void ac_schedule_print(ac_schedule_t *h);

#endif