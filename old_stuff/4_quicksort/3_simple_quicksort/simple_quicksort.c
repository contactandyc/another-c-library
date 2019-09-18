#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "timer.h"

#define sort_t char

void swap(sort_t *a, sort_t *b) {
  sort_t tmp = *a;
  *a = *b;
  *b = tmp;
}

int find_pivot(sort_t *low, sort_t *high) {
  sort_t *wp = low;
  sort_t *p = low;
  sort_t *mid = low + ((high-low)>>1);
  swap(mid, high);
  while(p < high) {
    if(*p < *high) {
      swap(p, wp);
      wp++;
    }
    p++;
  }
  swap(wp, high);
  return wp-low;
}

void quicksort(sort_t *low, sort_t *high) {
  if(low < high) {
    int pivot = find_pivot(low, high);
    quicksort(low, low+pivot-1);
    quicksort(low+pivot+1, high);
  }
}

int main( int argc, char *argv[]) {
  int repeat_test = 1000000;
  timer_t *overall_timer = timer_init(repeat_test);
  for( int i=1; i<argc; i++ ) {
    char *s = (char *)malloc(strlen(argv[i])+1);

    timer_t *copy_timer = timer_init(repeat_test);
    timer_start(copy_timer);
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
    }
    timer_end(copy_timer);

    timer_t *test_timer = timer_timer_init(copy_timer);
    timer_start(test_timer);
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
      quicksort(s, s+strlen(s)-1);
    }
    timer_end(test_timer);
    timer_add(overall_timer, test_timer);

    printf("%s => %s\n", argv[i], s);
    printf( "time_spent: %0.4fns\n", timer_ns(test_timer) );

    timer_destroy(test_timer);
    timer_destroy(copy_timer);
    free(s);
  }
  printf( "overall time_spent: %0.4fns\n", timer_ns(overall_timer) );
  timer_destroy(overall_timer);
  return 0;
}
