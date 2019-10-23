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

#include "ac_timer.h"
#include "ac_allocator.h"

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
  ac_timer_t *overall_timer = ac_timer_init(repeat_test);
  for( int i=1; i<argc; i++ ) {
    size_t len = strlen(argv[i]);
    char *s = (char *)ac_malloc(len+1);

    ac_timer_t *copy_timer = ac_timer_init(ac_timer_get_repeat(overall_timer));
    ac_timer_start(copy_timer);
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
    }
    ac_timer_stop(copy_timer);

    ac_timer_t *test_timer = ac_timer_init(ac_timer_get_repeat(overall_timer));
    ac_timer_start(test_timer);
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
      reverse_string(s, len);
    }
    ac_timer_stop(test_timer);
    ac_timer_subtract(test_timer, copy_timer);
    ac_timer_add(overall_timer, test_timer);

    printf("%s => %s\n", argv[i], s);
    printf( "time_spent: %0.4fns\n", ac_timer_ns(test_timer) );

    ac_timer_destroy(test_timer);
    // ac_timer_destroy(copy_timer);
    // ac_free(s);
  }
  printf( "overall time_spent: %0.4fns\n", ac_timer_ns(overall_timer) );
  ac_timer_destroy(overall_timer);
  return 0;
}
