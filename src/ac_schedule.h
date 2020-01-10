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

struct ac_task_state_link_s;
typedef struct ac_task_state_link_s ac_task_state_link_t;

struct ac_task_part_s;
typedef struct ac_task_part_s ac_task_part_t;

struct ac_task_output_s;
typedef struct ac_task_output_s ac_task_output_t;

struct ac_task_input_s;
typedef struct ac_task_input_s ac_task_input_t;

struct ac_task_link_s;
typedef struct ac_task_link_s ac_task_link_t;

struct ac_task_link_s {
  ac_task_t *task;
  ac_task_link_t *next;
};

typedef bool (*ac_task_f)(ac_task_t *task);
typedef bool (*ac_task_part_f)(ac_task_part_t *tp);
typedef bool (*ac_task_check_input_f)(ac_task_part_t *tp, ac_task_input_t *inp,
                                      time_t ack_time);

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

ac_task_output_t *ac_task_get_output(ac_task_part_t *tp, size_t pos);
ac_task_input_t *ac_task_get_input(ac_task_part_t *tp, size_t pos);

const char *ac_task_name(ac_task_t *task);

ac_schedule_t *ac_task_schedule(ac_task_t *task);

size_t ac_task_part_ram(ac_task_part_t *tp, double pct);
char *ac_task_output_base(ac_task_part_t *tp, ac_task_output_t *outp);
char *ac_task_output_base2(ac_task_part_t *tp, ac_task_output_t *outp,
                           const char *suffix);
char *ac_task_input_name(ac_task_part_t *tp, ac_task_input_t *inp,
                         size_t partition);

void ac_task_runner(ac_task_t *task, ac_task_part_f runner);

void ac_task_input_files(ac_task_t *task, const char *name, double ram_pct,
                         ac_task_check_input_f check);

static const size_t AC_OUTPUT_KEEP = 1;
static const size_t AC_OUTPUT_USE_FIRST = 2;
static const size_t AC_OUTPUT_SPLIT = 4;

void ac_task_output(ac_task_t *task, const char *name, const char *destinations,
                    double out_ram_pct, double in_ram_pct, size_t flags);

ac_schedule_t *ac_schedule_init(size_t num_partitions, size_t cpus, size_t ram,
                                size_t disk_space);
void ac_schedule_ack_dir(ac_schedule_t *h, const char *ack_dir);
void ac_schedule_task_dir(ac_schedule_t *h, const char *task_dir);

ac_task_t *ac_schedule_task(ac_schedule_t *h, const char *task_name,
                            bool partitioned, ac_task_f setup);

void ac_schedule_run(ac_schedule_t *h, ac_task_part_f on_complete);

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

void ac_schedule_print(ac_schedule_t *h);

void ac_schedule_destroy(ac_schedule_t *h);

#endif
