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

#ifndef _timer_H
#define _timer_H

struct timer_s;
typedef struct timer_s timer_t;

/*
   Initialize the timer.  repeat is necessary to indicate how many times the
   test was repeated so that the final result represents that.  If a thing is
   only being timed with a single repetition, then use a value of 1.
*/
timer_t *timer_init(int repeat);

/* destroy the timer */
void timer_destroy(timer_t *t);

/* get the number of times a task is meant to repeat */
int timer_get_repeat(timer_t *t);

/* set the number of times a task is meant to repeat */
void timer_set_repeat(timer_t *t, int repeat);

/* Subtract the amount of time spent in sub from the current timer */
void timer_subtract(timer_t *t, timer_t *sub);
/* Add the amount of time spent in add to the current timer */
void timer_add(timer_t *t, timer_t *add);

/* start the timer */
void timer_start(timer_t *t);

/* stop the timer */
void timer_stop(timer_t *t);

/*
  Returns time spent in nanoseconds (ns), microseconds (us), milliseconds (ms),
  and seconds (sec).
*/
double timer_ns(timer_t *t);
double timer_us(timer_t *t);
double timer_ms(timer_t *t);
double timer_sec(timer_t *t);

#endif
