#include "ac_allocator.h"
#include "ac_in.h"
#include "ac_out.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  int32_t user_id;
  int32_t item_id;
  int32_t rating;
  int32_t date;
} entry_t;

size_t split_by_user(const ac_io_record_t *r, size_t num_part, void *tag) {
  entry_t *entry = (entry_t *)r->record;
  return entry->user_id % num_part;
}

size_t split_by_item(const ac_io_record_t *r, size_t num_part, void *tag) {
  entry_t *entry = (entry_t *)r->record;
  return entry->item_id % num_part;
}

int compare_entry_by_user_and_item(ac_io_record_t *a, ac_io_record_t *b,
                                   void *tag) {
  entry_t *p1 = (entry_t *)a->record;
  entry_t *p2 = (entry_t *)b->record;
  if (p1->user_id != p2->user_id)
    return (p1->user_id < p2->user_id) ? -1 : 1;
  if (p1->item_id != p2->item_id)
    return (p1->item_id < p2->item_id) ? -1 : 1;
  return 0;
}

int compare_entry_by_user_and_time(ac_io_record_t *a, ac_io_record_t *b,
                                   void *tag) {
  entry_t *p1 = (entry_t *)a->record;
  entry_t *p2 = (entry_t *)b->record;
  if (p1->user_id != p2->user_id)
    return (p1->user_id < p2->user_id) ? -1 : 1;
  if (p1->date != p2->date)
    return (p1->date < p2->date) ? -1 : 1;
  return 0;
}

int compare_entry_by_user_time_and_item(ac_io_record_t *a, ac_io_record_t *b,
                                        void *tag) {
  entry_t *p1 = (entry_t *)a->record;
  entry_t *p2 = (entry_t *)b->record;
  if (p1->user_id != p2->user_id)
    return (p1->user_id < p2->user_id) ? -1 : 1;
  if (p1->date != p2->date)
    return (p1->date < p2->date) ? -1 : 1;
  if (p1->item_id != p2->item_id)
    return (p1->item_id < p2->item_id) ? -1 : 1;
  return 0;
}

int compare_entry_by_item_and_user(ac_io_record_t *a, ac_io_record_t *b,
                                   void *tag) {
  entry_t *p1 = (entry_t *)a->record;
  entry_t *p2 = (entry_t *)b->record;
  if (p1->item_id != p2->item_id)
    return (p1->item_id < p2->item_id) ? -1 : 1;
  if (p1->user_id != p2->user_id)
    return (p1->user_id < p2->user_id) ? -1 : 1;
  return 0;
}

ac_in_t *open_input(const char *in_file, size_t buffer_size) {
  ac_in_options_t in_opts;
  ac_in_options_init(&in_opts);
  ac_in_options_buffer_size(&in_opts, buffer_size);
  ac_in_options_format(&in_opts, ac_io_fixed(sizeof(entry_t)));
  return ac_in_init(in_file, &in_opts);
}

ac_out_t *open_output(const char *out_file, size_t buffer_size,
                      ac_io_compare_f compare, ac_io_partition_f partition,
                      size_t num_out) {
  ac_out_options_t out_opts;
  ac_out_options_init(&out_opts);
  ac_out_options_buffer_size(&out_opts, buffer_size);
  ac_out_options_format(&out_opts, ac_io_fixed(sizeof(entry_t)));

  ac_out_ext_options_t out_ext_opts;
  ac_out_ext_options_init(&out_ext_opts);
  ac_out_ext_options_use_extra_thread(&out_ext_opts);
  ac_out_ext_options_compare(&out_ext_opts, compare, NULL);
  if (partition) {
    ac_out_ext_options_partition(&out_ext_opts, partition, NULL);
    ac_out_ext_options_num_partitions(&out_ext_opts, num_out);
  }
  return ac_out_ext_init(out_file, &out_opts, &out_ext_opts);
}

/* 1. partition number, send to all, send to part
   2. thread number, send to all, send to part
   3. thread number,
*/

void sort_entries_by_user(const char *in_file, const char *out_file,
                          size_t num_out, size_t max_per_day) {
  ac_in_t *in = open_input(in_file, 2 * 1024 * 1024);
  // ac_in_limit(in, 100000);
  /*
  ac_in_transform_format(in, ac_io_fixed(sizeof(entry_t)));
  ac_in_transform_buffer_size(in, 30 * 1024 * 1024);
  ac_in_transform_compare(in, compare_entry_by_user_and_item, NULL);
  ac_in_transform_reducer(in, ac_io_keep_first, NULL);
  ac_in_transform_add(in, in) in = ac_in_transform(in);
  */
  in = ac_in_transform(in, ac_io_fixed(sizeof(entry_t)), 30 * 1024 * 1024,
                       compare_entry_by_user_and_item, NULL, ac_io_keep_first,
                       NULL, NULL, NULL);

  in = ac_in_transform(in, ac_io_fixed(sizeof(entry_t)), 30 * 1024 * 1024,
                       compare_entry_by_user_time_and_item, NULL, NULL, NULL,
                       NULL, NULL);

  ac_out_t *out =
      open_output(out_file, 30 * 1024 * 1024, compare_entry_by_user_and_item,
                  split_by_item, num_out);
  ac_io_record_t *r;
  size_t num_r = 0;
  while ((r = ac_in_advance_group(in, &num_r, compare_entry_by_user_and_time,
                                  NULL)) != NULL) {
    if (num_r > max_per_day)
      continue;

    ac_io_record_t *rp = r;
    ac_io_record_t *ep = r + num_r;
    while (rp < ep) {
      ac_out_write_record(out, rp->record, rp->length);
      rp++;
    }
  }
  ac_in_destroy(in);
  ac_out_destroy(out);
}

int usage(const char *prog) {
  printf("%s <input_file> <out_by_item> <out_by_user>\n", prog);
  return -1;
}

/* in order for correlations to work, the following transformations need done
  1. sort by user and time
     a. group by user and drop records where there are more than 5 per day
     b. sort by user and item and write to file split by item
  2. sort each of the files that are sorted by user and item by item and date
  3. do a scan of each of the files and match against all other files
     writing item1, item2, frequency for top N sorting by item1, item2 and
     splitting by item1.
  4. merge files split by item1
*/

int main(int argc, char *argv[]) {
  if (argc < 4)
    return usage(argv[0]);

  char *in_file = argv[1];

  char *out_by_item = argv[2];
  char *out_by_user = argv[3];

  // sort_entries_by_item(in_file, out_by_item);
  sort_entries_by_user(in_file, out_by_user, 16, 5);
  return 0;
}
