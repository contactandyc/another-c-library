#ifndef _stla_timer_H
#define _stla_timer_H

struct stla_timer_s;
typedef struct stla_timer_s stla_timer_t;

/*
   Initialize the timer.  repeat is necessary to indicate how many times the
   test was repeated so that the final result represents that.  If a thing is
   only being timed with a single repetition, then use a value of 1.
*/
stla_timer_t *stla_timer_init(int repeat);

/* destroy the timer */
void stla_timer_destroy(stla_timer_t *t);

/* get the number of times a task is meant to repeat */
int stla_timer_get_repeat(stla_timer_t *t);

/* set the number of times a task is meant to repeat */
void stla_timer_set_repeat(stla_timer_t *t, int repeat);

/* Subtract the amount of time spent in sub from the current timer */
void stla_timer_subtract(stla_timer_t *t, stla_timer_t *sub);
/* Add the amount of time spent in add to the current timer */
void stla_timer_add(stla_timer_t *t, stla_timer_t *add);

/* start the timer */
void stla_timer_start(stla_timer_t *t);

/* stop the timer */
void stla_timer_stop(stla_timer_t *t);

/*
  Returns time spent in nanoseconds (ns), microseconds (us), milliseconds (ms),
  and seconds (sec).
*/
double stla_timer_ns(stla_timer_t *t);
double stla_timer_us(stla_timer_t *t);
double stla_timer_ms(stla_timer_t *t);
double stla_timer_sec(stla_timer_t *t);

#endif
