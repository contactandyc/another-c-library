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
