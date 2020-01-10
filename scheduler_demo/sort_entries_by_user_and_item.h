#ifndef _sort_entries_by_user_and_item_H
#define _sort_entries_by_user_and_item_H

#include "ac_schedule.h"

/* Take the unsorted entry_t records from convert_ratings and ...
   1. sort by user and time and eliminate days that have more than 5 ratings.
   2. sort by user and item eliminating duplicate user/item pairs.
   3. write user/item records and split by item
*/
bool sort_entries_by_user_and_item(ac_task_part_t *tp);

/* Merge the result of sort_entries_by_user_and_item into a single file from
   across the partitions and write another single file sorted by item and user.
*/
bool merge_entries_by_user_and_item(ac_task_part_t *tp);

#endif
