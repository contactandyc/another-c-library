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

#include "actimer.h"

#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

struct actimer_s {
  long base;
  int repeat;
  long time_spent;
  long start_time;
};

actimer_t *actimer_init(int repeat) {
  actimer_t *t = (actimer_t *)malloc(sizeof(actimer_t));
  t->repeat = repeat;
  t->base = t->time_spent = t->start_time = 0;
  return t;
}

void actimer_destroy(actimer_t *t) {
  free(t);
}

/* get the number of times a task is meant to repeat */
int actimer_get_repeat(actimer_t *t) {
  return t->repeat;
}

/* set the number of times a task is meant to repeat */
void actimer_set_repeat(actimer_t *t, int repeat) {
  t->repeat = repeat;
}


void actimer_subtract(actimer_t *t, actimer_t *sub) {
  t->base -= (sub->time_spent+sub->base);
}

void actimer_add(actimer_t *t, actimer_t *add) {
  t->base += (add->time_spent+add->base);
}

void actimer_start(actimer_t *t) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  t->start_time = (tv.tv_sec * 1000000) + tv.tv_usec;
}

void actimer_stop(actimer_t *t) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  long v = (tv.tv_sec * 1000000) + tv.tv_usec;
  v -= t->start_time;
  t->time_spent += v;
}

double actimer_ns(actimer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return (ts*1000.0) / r;
}

double actimer_us(actimer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / r;
}

double actimer_ms(actimer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / (r*1000.0);
}

double actimer_sec(actimer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / (r*1000000.0);
}
