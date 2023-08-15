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

#include "ac_allocator.h"
#include "ac_timer.h"

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

struct ac_timer_s {
  long base;
  int repeat;
  long time_spent;
  long start_time;
};

#ifdef _AC_DEBUG_MEMORY_
ac_timer_t *_ac_timer_init(int repeat, const char *caller) {
  ac_timer_t *t =
      (ac_timer_t *)_ac_malloc_d(NULL, caller, sizeof(ac_timer_t), false);
#else
ac_timer_t *_ac_timer_init(int repeat) {
  ac_timer_t *t = (ac_timer_t *)ac_malloc(sizeof(ac_timer_t));
#endif
  t->repeat = repeat;
  t->base = t->time_spent = t->start_time = 0;
  return t;
}

void ac_timer_destroy(ac_timer_t *t) { ac_free(t); }

/* get the number of times a task is meant to repeat */
int ac_timer_get_repeat(ac_timer_t *t) { return t->repeat; }

/* set the number of times a task is meant to repeat */
void ac_timer_set_repeat(ac_timer_t *t, int repeat) { t->repeat = repeat; }

void ac_timer_subtract(ac_timer_t *t, ac_timer_t *sub) {
  t->base -= (sub->time_spent + sub->base);
}

void ac_timer_add(ac_timer_t *t, ac_timer_t *add) {
  t->base += (add->time_spent + add->base);
}

long ac_timer_current() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000000) + tv.tv_usec;
}

void ac_timer_start(ac_timer_t *t) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  t->start_time = (tv.tv_sec * 1000000) + tv.tv_usec;
}

void ac_timer_stop(ac_timer_t *t) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  long v = (tv.tv_sec * 1000000) + tv.tv_usec;
  v -= t->start_time;
  t->time_spent += v;
}

double ac_timer_ns(ac_timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return (ts * 1000.0) / r;
}

double ac_timer_us(ac_timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / r;
}

double ac_timer_ms(ac_timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / (r * 1000.0);
}

double ac_timer_sec(ac_timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / (r * 1000000.0);
}
