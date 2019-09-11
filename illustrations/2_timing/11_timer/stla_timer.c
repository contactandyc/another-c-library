#include "stla_timer.h"

#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

struct stla_timer_s {
  long base;
  int repeat;
  long time_spent;
  long start_time;
};

stla_timer_t *stla_timer_init(int repeat) {
  stla_timer_t *t = (stla_timer_t *)malloc(sizeof(stla_timer_t));
  t->repeat = repeat;
  t->base = t->time_spent = t->start_time = 0;
  return t;
}

void stla_timer_destroy(stla_timer_t *t) {
  free(t);
}

/* get the number of times a task is meant to repeat */
int stla_timer_get_repeat(stla_timer_t *t) {
  return t->repeat;
}

/* set the number of times a task is meant to repeat */
void stla_timer_set_repeat(stla_timer_t *t, int repeat) {
  t->repeat = repeat;
}


void stla_timer_subtract(stla_timer_t *t, stla_timer_t *sub) {
  t->base -= (sub->time_spent+sub->base);
}

void stla_timer_add(stla_timer_t *t, stla_timer_t *add) {
  t->base += (add->time_spent+add->base);
}

void stla_timer_start(stla_timer_t *t) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  t->start_time = (tv.tv_sec * 1000000) + tv.tv_usec;
}

void stla_timer_stop(stla_timer_t *t) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  long v = (tv.tv_sec * 1000000) + tv.tv_usec;
  v -= t->start_time;
  t->time_spent += v;
}

double stla_timer_ns(stla_timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return (ts*1000.0) / r;
}

double stla_timer_us(stla_timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / r;
}

double stla_timer_ms(stla_timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / (r*1000.0);
}

double stla_timer_sec(stla_timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / (r*1000000.0);
}
