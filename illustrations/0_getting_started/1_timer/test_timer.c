#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

void reverse_string( char *s ) {
  size_t len = strlen(s);
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
    char *s = (char *)malloc(strlen(argv[i])+1);
    long test_t1 = get_time();
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
      reverse_string(s);
    }
    long test_t2 = get_time();
    long time_spent = test_t2-test_t1;
    overall_time += time_spent;

    printf("%s => %s\n", argv[i], s);
    printf( "time_spent: %0.4fns\n", (time_spent*1000.0)/(repeat_test*1.0));

    free(s);
  }
  printf( "overall time_spent: %0.4fns\n", (overall_time*1000.0)/(repeat_test*1.0));
  return 0;
}
