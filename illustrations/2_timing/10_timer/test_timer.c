#include "timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reverse_string( char *s, size_t len ) {
  char *e = s+len-1;
  while(s < e) {
    char tmp = *s;
    *s = *e;
    *e = tmp;
    s++;
    e--;
  }
}

int main( int argc, char *argv[]) {
  int repeat_test = 1000000;
  timer_t *overall_timer = timer_init(repeat_test);
  for( int i=1; i<argc; i++ ) {
    size_t len = strlen(argv[i]);
    char *s = (char *)malloc(len+1);

    timer_t *copy_timer = timer_init(timer_get_repeat(overall_timer));
    timer_start(copy_timer);
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
    }
    timer_stop(copy_timer);

    timer_t *test_timer = timer_init(timer_get_repeat(overall_timer));
    timer_start(test_timer);
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
      reverse_string(s, len);
    }
    timer_stop(test_timer);
    timer_subtract(test_timer, copy_timer);
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