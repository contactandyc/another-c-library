#include "ac_schedule.h"
#include "aux_methods.h"
#include "convert_ratings.h"
#include "sort_entries_by_user_and_item.h"

#include <stdio.h>

bool task_completed(ac_task_part_t *tp) {
  printf("Finished %s[%lu] on thread %lu\n", ac_task_name(tp->task),
         tp->partition, tp->thread_id);
  return true;
}

bool setup_convert_ratings(ac_task_t *task) {
  ac_task_runner(task, convert_ratings);
  ac_task_input_files(task, "mv", 0.05, check_mv_files);
  ac_task_output(task, "by_user.lz4", "sort_by_user_and_item", 0.95, 0.10,
                 AC_OUTPUT_KEEP | AC_OUTPUT_SPLIT);
  return true;
}

bool setup_sort_by_user_and_item(ac_task_t *task) {
  // this shouldn't be needed once input dependencies are resolved
  ac_task_dependency(task, "convert_ratings");
  ac_task_runner(task, sort_entries_by_user_and_item);
  ac_task_output(task, "user_item.lz4", "merge_by_user_and_item", 0.95, 0.10,
                 AC_OUTPUT_KEEP | AC_OUTPUT_SPLIT);
  return true;
}

bool setup_merge_by_user_and_item(ac_task_t *task) {
  ac_task_dependency(task, "sort_by_user_and_item");
  ac_task_runner(task, merge_entries_by_user_and_item);
  ac_task_output(task, "user_item.lz4", NULL, 0.4, 0.10, AC_OUTPUT_KEEP);
  ac_task_output(task, "item_user.lz4", NULL, 0.4, 0.10, AC_OUTPUT_KEEP);
  return true;
}

int main(int argc, char *argv[]) {
  ac_schedule_t *scheduler = ac_schedule_init(16, 16, 1000, 100);
  ac_schedule_task(scheduler, "convert_ratings", true, setup_convert_ratings);
  ac_schedule_task(scheduler, "sort_by_user_and_item", true,
                   setup_sort_by_user_and_item);
  ac_schedule_task(scheduler, "merge_by_user_and_item", true,
                   setup_merge_by_user_and_item);
  ac_schedule_run(scheduler, task_completed);
  ac_schedule_destroy(scheduler);
  return 0;
}
