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
