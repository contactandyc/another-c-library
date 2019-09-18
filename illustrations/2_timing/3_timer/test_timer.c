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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

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

long get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000000) + tv.tv_usec;
}

int main( int argc, char *argv[]) {
  int repeat_test = 1000000;
  long overall_time = 0;
  for( int i=1; i<argc; i++ ) {
    size_t len = strlen(argv[i]);
    char *s = (char *)malloc(len+1);

    long copy_t1 = get_time();
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
    }
    long copy_t2 = get_time();

    long test_t1 = get_time();
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
      reverse_string(s, len);
    }
    long test_t2 = get_time();
    long time_spent = (test_t2-test_t1) - (copy_t2-copy_t1);
    overall_time += time_spent;

    printf("%s => %s\n", argv[i], s);
    printf( "time_spent: %0.4fns\n", (time_spent*1000.0)/(repeat_test*1.0));

    free(s);
  }
  printf( "overall time_spent: %0.4fns\n", (overall_time*1000.0)/(repeat_test*1.0));
  return 0;
}
