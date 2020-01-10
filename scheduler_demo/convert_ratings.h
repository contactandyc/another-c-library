#ifndef _convert_ratings_H
#define _convert_ratings_H

#include "ac_schedule.h"

/* This will convert text files from netflix into entry_t structures defined
   in aux_methods.h */
bool convert_ratings(ac_task_part_t *tp);

/* This checks to see if the input is newer than the ack_time.  Because the
   input data isn't likely to change, this function is here mostly for
   completeness. */
bool check_mv_files(ac_task_part_t *tp, ac_task_input_t *inp, time_t ack_time);

#endif
