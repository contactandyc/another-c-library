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

/*
   Initialize a timer from another timer.  This will subtract the time spent
   from the other timer and set the repeat to match the other timer.
*/
timer_t *timer_timer_init(timer_t *t);

/* destroy the timer */
void timer_destroy(timer_t *t);

/* Subtract the amount of time spent in sub from the current timer */
void timer_subtract(timer_t *t, timer_t *sub);
/* Add the amount of time spent in add to the current timer */
void timer_add(timer_t *t, timer_t *add);

/* start the timer */
void timer_start(timer_t *t);

/* end the timer */
void timer_end(timer_t *t);

/*
  Returns time spent in nanoseconds (ns), microseconds (us), milliseconds (ms),
  and seconds (sec).
*/
double timer_ns(timer_t *t);
double timer_us(timer_t *t);
double timer_ms(timer_t *t);
double timer_sec(timer_t *t);

#endif
