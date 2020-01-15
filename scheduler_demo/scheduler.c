#include "ac_schedule.h"
#include "aux_methods.h"

#include <stdio.h>

bool task_completed(ac_worker_t *tp) {
  printf("Finished %s[%lu] on thread %lu in %0.3fms\n", ac_task_name(tp->task),
         tp->partition, tp->thread_id, ac_timer_ms(tp->timer));
  return true;
}

ac_io_file_info_t *get_mv_files(ac_worker_t *tp, size_t *num_files,
                                ac_worker_input_t *inp) {
  ac_io_file_info_t *res = (ac_io_file_info_t *)ac_pool_alloc(
      tp->pool,
      sizeof(ac_io_file_info_t) * ((NUM_INPUT_MV / tp->num_partitions) + 2));
  ac_io_file_info_t *rp = res;
  for (size_t i = tp->partition; i < NUM_INPUT_MV; i += tp->num_partitions) {
    rp->filename = ac_pool_strdupf(tp->pool, INPUT_MV, i);
    rp->tag = i;
    if (ac_io_file_info(rp) && rp->size)
      rp++;
  }
  *num_files = rp - res;
  return res;
}

void convert_text_ratings_to_entry(ac_worker_t *w, ac_io_record_t *r,
                                   ac_out_t **out) {
  int user_id, rating, year, month, day;
  if (sscanf(r->record, "%d,%d,%d-%d-%d", &user_id, &rating, &year, &month,
             &day) != 5)
    return;
  entry_t entry;
  entry.date = (year * 10000) + (month * 100) + day;
  entry.rating = rating;
  entry.user_id = user_id;
  entry.item_id = r->tag;
  // printf("%d,%d,%d,%d\n", entry.user_id, entry.item_id, entry.rating,
  //         entry.date);
  ac_out_write_record(*out, &entry, sizeof(entry));
}

bool setup_convert_ratings(ac_task_t *task) {
  /* specify input_files (usually from outside) and only needed by bootstrap
     tasks. */
  ac_task_input_files(task, "mv", 0.05, get_mv_files);
  ac_task_input_format(task, ac_io_delimiter('\n'));
  ac_task_input_limit(task, 1000);

  /* specify output */
  ac_task_output(task, "by_user.lz4", "sort_by_user_and_item", 0.95, 0.10,
                 AC_OUTPUT_KEEP | AC_OUTPUT_SPLIT);
  ac_task_output_format(task, ac_io_fixed(sizeof(entry_t)));
  ac_task_output_compare(task, compare_entry_by_user_and_item, NULL);
  ac_task_output_partition(task, split_by_user, NULL);
  ac_task_output_keep_first(task);

  /* use in out runner */
  ac_task_default_runner(task);
  ac_task_transform(task, "mv", "by_user.lz4", convert_text_ratings_to_entry);
  return true;
}

void process_group(ac_worker_t *w, ac_io_record_t *r, size_t num_r,
                   ac_out_t **out) {
  if (num_r > MAX_RATINGS_PER_DAY)
    return;

  ac_io_record_t *rp = r;
  ac_io_record_t *ep = r + num_r;
  while (rp < ep) {
    ac_out_write_record(*out, rp->record, rp->length);
    rp++;
  }
}

bool setup_sort_by_user_and_item(ac_task_t *task) {
  /* input is specified automatically by setup_convert_ratings ac_task_output */
  /* specify output */
  ac_task_output(task, "user_item_sorted.lz4", "merge_by_user_and_item", 0.95,
                 0.10, AC_OUTPUT_KEEP | AC_OUTPUT_SPLIT);
  ac_task_output_format(task, ac_io_fixed(sizeof(entry_t)));
  ac_task_output_compare(task, compare_entry_by_user_and_item, NULL);
  ac_task_output_partition(task, split_by_item, NULL);

  /* use in out runner */
  ac_task_default_runner(task);
  ac_task_group_transform(task, "by_user.lz4", "user_item_sorted.lz4",
                          process_group, compare_entry_by_user_and_time);
  return true;
}

bool setup_merge_by_user_and_item(ac_task_t *task) {
  ac_task_output(task, "user_item.lz4", NULL, 0.20, 0.10, AC_OUTPUT_KEEP);
  ac_task_output_format(task, ac_io_fixed(sizeof(entry_t)));

  ac_task_output(task, "item_user.lz4", NULL, 0.70, 0.10, AC_OUTPUT_KEEP);
  ac_task_output_format(task, ac_io_fixed(sizeof(entry_t)));
  ac_task_output_compare(task, compare_entry_by_item_and_user, NULL);

  ac_task_default_runner(task);
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
