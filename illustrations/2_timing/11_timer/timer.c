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

#include "timer.h"

#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

struct timer_s {
  long base;
  int repeat;
  long time_spent;
  long start_time;
};

timer_t *timer_init(int repeat) {
  timer_t *t = (timer_t *)malloc(sizeof(timer_t));
  t->repeat = repeat;
  t->base = t->time_spent = t->start_time = 0;
  return t;
}

void timer_destroy(timer_t *t) {
  free(t);
}

/* get the number of times a task is meant to repeat */
int timer_get_repeat(timer_t *t) {
  return t->repeat;
}

/* set the number of times a task is meant to repeat */
void timer_set_repeat(timer_t *t, int repeat) {
  t->repeat = repeat;
}


void timer_subtract(timer_t *t, timer_t *sub) {
  t->base -= (sub->time_spent+sub->base);
}

void timer_add(timer_t *t, timer_t *add) {
  t->base += (add->time_spent+add->base);
}

void timer_start(timer_t *t) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  t->start_time = (tv.tv_sec * 1000000) + tv.tv_usec;
}

void timer_stop(timer_t *t) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  long v = (tv.tv_sec * 1000000) + tv.tv_usec;
  v -= t->start_time;
  t->time_spent += v;
}

double timer_ns(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return (ts*1000.0) / r;
}

double timer_us(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / r;
}

double timer_ms(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / (r*1000.0);
}

double timer_sec(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / (r*1000000.0);
}
