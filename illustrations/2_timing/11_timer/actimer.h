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

#ifndef _actimer_H
#define _actimer_H

struct actimer_s;
typedef struct actimer_s actimer_t;

/*
   Initialize the timer.  repeat is necessary to indicate how many times the
   test was repeated so that the final result represents that.  If a thing is
   only being timed with a single repetition, then use a value of 1.
*/
actimer_t *actimer_init(int repeat);

/* destroy the timer */
void actimer_destroy(actimer_t *t);

/* get the number of times a task is meant to repeat */
int actimer_get_repeat(actimer_t *t);

/* set the number of times a task is meant to repeat */
void actimer_set_repeat(actimer_t *t, int repeat);

/* Subtract the amount of time spent in sub from the current timer */
void actimer_subtract(actimer_t *t, actimer_t *sub);
/* Add the amount of time spent in add to the current timer */
void actimer_add(actimer_t *t, actimer_t *add);

/* start the timer */
void actimer_start(actimer_t *t);

/* stop the timer */
void actimer_stop(actimer_t *t);

/*
  Returns time spent in nanoseconds (ns), microseconds (us), milliseconds (ms),
  and seconds (sec).
*/
double actimer_ns(actimer_t *t);
double actimer_us(actimer_t *t);
double actimer_ms(actimer_t *t);
double actimer_sec(actimer_t *t);

#endif
