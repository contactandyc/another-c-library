#include "sort_entries_by_user_and_item.h"

#include "aux_methods.h"

bool merge_entries_by_user_and_item(ac_task_part_t *tp) {
  ac_task_input_t *inp = ac_task_get_input(tp, 0);
  ac_in_options_t in_opts;
  ac_in_options_init(&in_opts);
  ac_in_options_format(&in_opts, ac_io_prefix());
  ac_in_t *in = ac_in_ext_init(compare_entry_by_user_and_item, NULL, &in_opts);
  size_t in_buffer_size = ac_task_part_ram(tp, inp->ram_pct * 0.20);

  size_t num_partitions = inp->src->num_partitions;
  for (size_t i = 0; i < num_partitions; i++) {
    ac_in_ext_add(in,
                  open_fixed_input(ac_task_input_name(tp, inp, i),
                                   sizeof(entry_t),
                                   in_buffer_size / num_partitions),
                  i);
  }

  ac_task_output_t *outp = ac_task_get_output(tp, 0);
  ac_out_t *out =
      open_output(ac_task_output_base(tp, outp),
                  ac_task_part_ram(tp, outp->ram_pct), NULL, NULL, 0, NULL);

  outp = ac_task_get_output(tp, 1);
  ac_out_t *out2 = open_output(ac_task_output_base(tp, outp),
                               ac_task_part_ram(tp, outp->ram_pct),
                               compare_entry_by_item_and_user, NULL, 0, NULL);

  size_t num_records = 0;
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL) {
    num_records++;
    ac_out_write_record(out, r->record, r->length);
    ac_out_write_record(out2, r->record, r->length);
  }
  // printf("%lu record(s)\n", num_records);
  ac_in_destroy(in);
  ac_out_destroy(out);
  ac_out_destroy(out2);
  return true;
}

bool sort_entries_by_user_and_item(ac_task_part_t *tp) {
  ac_task_input_t *inp = ac_task_get_input(tp, 0);
  ac_task_output_t *outp = ac_task_get_output(tp, 0);
  size_t num_partitions = inp->src->num_partitions;

  /* sort the data by user, time, and item and get cursor */
  const char *filename = ac_task_output_base2(tp, outp, "_stage1");
  ac_out_t *out =
      open_output(filename, ac_task_part_ram(tp, outp->ram_pct * 0.50),
                  compare_entry_by_user_time_and_item, NULL, 0, NULL);
  ac_io_record_t *r;
  for (size_t i = 0; i < num_partitions; i++) {
    filename = ac_task_input_name(tp, inp, i);
    ac_in_t *in = open_fixed_input(filename, sizeof(entry_t),
                                   ac_task_part_ram(tp, inp->ram_pct));
    while ((r = ac_in_advance(in)) != NULL)
      ac_out_write_record(out, r->record, r->length);

    ac_in_destroy(in);
  }

  filename = ac_task_output_base(tp, outp);
  ac_out_t *out2 =
      open_output(filename, ac_task_part_ram(tp, outp->ram_pct * 0.50),
                  compare_entry_by_user_and_item, split_by_item,
                  tp->num_partitions, ac_io_keep_first);

  /* get cursor */
  ac_in_t *in = ac_out_in(out);
  size_t num_r = 0;
  while ((r = ac_in_advance_group(in, &num_r, compare_entry_by_user_and_time,
                                  NULL)) != NULL) {
    if (num_r > MAX_RATINGS_PER_DAY)
      continue;

    ac_io_record_t *rp = r;
    ac_io_record_t *ep = r + num_r;
    while (rp < ep) {
      ac_out_write_record(out2, rp->record, rp->length);
      rp++;
    }
  }
  ac_in_destroy(in);
  ac_out_destroy(out);
  ac_out_destroy(out2);
  return true;
}
