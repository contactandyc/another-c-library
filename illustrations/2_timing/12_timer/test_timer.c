#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stla_timer.h"

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
  stla_timer_t *overall_timer = stla_timer_init(repeat_test);
  for( int i=1; i<argc; i++ ) {
    size_t len = strlen(argv[i]);
    char *s = (char *)malloc(len+1);

    stla_timer_t *copy_timer = stla_timer_init(repeat_test);
    stla_timer_start(copy_timer);
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
    }
    stla_timer_end(copy_timer);

    stla_timer_t *test_timer = stla_timer_timer_init(copy_timer);
    stla_timer_start(test_timer);
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
      reverse_string(s, len);
    }
    stla_timer_end(test_timer);
    stla_timer_add(overall_timer, test_timer);

    printf("%s => %s\n", argv[i], s);
    printf( "time_spent: %0.4fns\n", stla_timer_ns(test_timer) );

    stla_timer_destroy(test_timer);
    stla_timer_destroy(copy_timer);
    free(s);
  }
  printf( "overall time_spent: %0.4fns\n", stla_timer_ns(overall_timer) );
  stla_timer_destroy(overall_timer);
  return 0;
}
