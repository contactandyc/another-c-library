#ifndef _aux_methods_H
#define _aux_methods_H

#include <inttypes.h>

#include "ac_in.h"
#include "ac_io.h"
#include "ac_out.h"

#define INPUT_MV "training_set/mv_%07lu.txt"
#define NUM_INPUT_MV 17770
#define MAX_RATINGS_PER_DAY 5

typedef struct {
  int32_t user_id;
  int32_t item_id;
  int32_t rating;
  int32_t date;
} entry_t;

static inline size_t split_by_user(const ac_io_record_t *r, size_t num_part,
                                   void *tag) {
  entry_t *entry = (entry_t *)r->record;
  return entry->user_id % num_part;
}

static inline size_t split_by_item(const ac_io_record_t *r, size_t num_part,
                                   void *tag) {
  entry_t *entry = (entry_t *)r->record;
  return entry->item_id % num_part;
}

static inline int compare_entry_by_user_and_item(ac_io_record_t *a,
                                                 ac_io_record_t *b, void *tag) {
  entry_t *p1 = (entry_t *)a->record;
  entry_t *p2 = (entry_t *)b->record;
  if (p1->user_id != p2->user_id)
    return (p1->user_id < p2->user_id) ? -1 : 1;
  if (p1->item_id != p2->item_id)
    return (p1->item_id < p2->item_id) ? -1 : 1;
  return 0;
}

static inline int compare_entry_by_user_and_time(ac_io_record_t *a,
                                                 ac_io_record_t *b, void *tag) {
  entry_t *p1 = (entry_t *)a->record;
  entry_t *p2 = (entry_t *)b->record;
  if (p1->user_id != p2->user_id)
    return (p1->user_id < p2->user_id) ? -1 : 1;
  if (p1->date != p2->date)
    return (p1->date < p2->date) ? -1 : 1;
  return 0;
}

static inline int compare_entry_by_user_time_and_item(ac_io_record_t *a,
                                                      ac_io_record_t *b,
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

static inline int compare_entry_by_item_and_user(ac_io_record_t *a,
                                                 ac_io_record_t *b, void *tag) {
  entry_t *p1 = (entry_t *)a->record;
  entry_t *p2 = (entry_t *)b->record;
  if (p1->item_id != p2->item_id)
    return (p1->item_id < p2->item_id) ? -1 : 1;
  if (p1->user_id != p2->user_id)
    return (p1->user_id < p2->user_id) ? -1 : 1;
  return 0;
}

ac_out_t *open_output(const char *out_file, size_t buffer_size,
                      ac_io_compare_f compare, ac_io_partition_f partition,
                      size_t num_out, ac_io_reducer_f reducer);

ac_in_t *open_fixed_input(const char *in_file, size_t fixed_size,
                          size_t buffer_size);

#endif
