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
