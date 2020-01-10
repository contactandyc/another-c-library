#include "convert_ratings.h"

#include "aux_methods.h"

bool convert_ratings(ac_task_part_t *tp) {
  ac_task_input_t *inp = ac_task_get_input(tp, 0);

  ac_in_options_t in_opts;
  ac_in_options_init(&in_opts);
  ac_in_options_buffer_size(&in_opts, ac_task_part_ram(tp, inp->ram_pct));
  ac_in_options_format(&in_opts, ac_io_delimiter('\n'));

  ac_task_output_t *outp = ac_task_get_output(tp, 0);
  ac_out_t *out = open_output(ac_task_output_base(tp, outp),
                              ac_task_part_ram(tp, outp->ram_pct), NULL,
                              split_by_user, tp->num_partitions, NULL);

  for (size_t i = tp->partition; i < NUM_INPUT_MV; i += tp->num_partitions) {
    ac_buffer_setf(tp->bh, INPUT_MV, i);
    char *filename = ac_buffer_data(tp->bh);
    // printf("%s\n", filename);
    ac_in_t *in = ac_in_init(filename, &in_opts);
    ac_io_record_t *r = ac_in_advance(in);
    if (r) {
      int user_id, rating, year, month, day;
      while ((r = ac_in_advance(in))) {
        if (sscanf(r->record, "%d,%d,%d-%d-%d", &user_id, &rating, &year,
                   &month, &day) != 5)
          abort();
        entry_t entry;
        entry.date = (year * 10000) + (month * 100) + day;
        entry.rating = rating;
        entry.user_id = user_id;
        entry.item_id = i;
        //  printf("%d,%d,%d,%d\n", entry.user_id, entry.item_id, entry.rating,
        //         entry.date);
        ac_out_write_record(out, &entry, sizeof(entry));
      }
    }
    ac_in_destroy(in);
  }
  ac_out_destroy(out);
  return true;
}

bool check_mv_files(ac_task_part_t *tp, ac_task_input_t *inp, time_t ack_time) {
  // check if input files are newer than ack file
  for (size_t i = tp->partition; i < NUM_INPUT_MV; i += tp->num_partitions) {
    ac_buffer_setf(tp->bh, INPUT_MV, i);
    char *filename = ac_buffer_data(tp->bh);
    if (ac_io_modified(filename) > ack_time)
      return true;
  }
  return false;
}
