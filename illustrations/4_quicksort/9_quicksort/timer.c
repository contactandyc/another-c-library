#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

#include "timer.h"

struct timer_s {
  int repeat;
  long time_spent;
  long start_time;
};

timer_t *timer_init(int repeat) {
  timer_t *t = (timer_t *)malloc(sizeof(timer_t));
  t->repeat = repeat;
  t->time_spent = t->start_time = 0;
  return t;
}

timer_t *timer_timer_init(timer_t *t) {
  timer_t *r = (timer_t *)malloc(sizeof(timer_t));
  r->repeat = t->repeat;
  r->time_spent = -t->time_spent;
  r->start_time = 0;
  return r;
}

void timer_destroy(timer_t *t) {
  free(t);
}

void timer_subtract(timer_t *t, timer_t *sub) {
  t->time_spent -= sub->time_spent;
}

void timer_add(timer_t *t, timer_t *add) {
  t->time_spent += add->time_spent;
}

void timer_start(timer_t *t) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  t->start_time = (tv.tv_sec * 1000000) + tv.tv_usec;
}

void timer_end(timer_t *t) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  long v = (tv.tv_sec * 1000000) + tv.tv_usec;
  v -= t->start_time;
  t->time_spent += v;
}

double timer_ns(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent;
  return (ts*1000.0) / r;
}

double timer_mis(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent;
  return ts / r;
}

double timer_ms(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent;
  return ts / (r*1000.0);
}

double timer_sec(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent;
  return ts / (r*1000000.0);
}
