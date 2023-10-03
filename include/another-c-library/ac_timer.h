/*
Copyright 2019 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _ac_timer_H
#define _ac_timer_H

#include "another-c-library/ac_common.h"
#include "another-c-library/ac_pool.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ac_timer_s;
typedef struct ac_timer_s ac_timer_t;

/* returns the time with ms precision */
long ac_timer_current();

/*
   Initialize the timer.  repeat is necessary to indicate how many times the
   test was repeated so that the final result represents that.  If a thing is
   only being timed with a single repetition, then use a value of 1.
*/
#ifdef _AC_MEMORY_CHECK_
#define ac_timer_init(repeat)                                                  \
  _ac_timer_init(repeat, AC_FILE_LINE_MACRO("ac_timer"))
ac_timer_t *_ac_timer_init(int repeat, const char *caller);
#else
#define ac_timer_init(repeat) _ac_timer_init(repeat)
ac_timer_t *_ac_timer_init(int repeat);
#endif

ac_timer_t *ac_timer_pool_init(ac_pool_t *pool, int repeat);

/* destroy the timer */
void ac_timer_destroy(ac_timer_t *t);

/* get the number of times a task is meant to repeat */
int ac_timer_get_repeat(ac_timer_t *t);

/* set the number of times a task is meant to repeat */
void ac_timer_set_repeat(ac_timer_t *t, int repeat);

/* Subtract the amount of time spent in sub from the current timer */
void ac_timer_subtract(ac_timer_t *t, ac_timer_t *sub);
/* Add the amount of time spent in add to the current timer */
void ac_timer_add(ac_timer_t *t, ac_timer_t *add);

/* start the timer */
void ac_timer_start(ac_timer_t *t);

/* stop the timer */
void ac_timer_stop(ac_timer_t *t);

/*
  Returns time spent in nanoseconds (ns), microseconds (us), milliseconds (ms),
  and seconds (sec).
*/
double ac_timer_ns(ac_timer_t *t);
double ac_timer_us(ac_timer_t *t);
double ac_timer_ms(ac_timer_t *t);
double ac_timer_sec(ac_timer_t *t);

#ifdef __cplusplus
}
#endif

#endif
