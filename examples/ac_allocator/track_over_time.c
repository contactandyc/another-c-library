#include "ac_allocator.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("Demo to show how allocations are tracked\n");
  int tests = 32;
  int speed = 10;
  char **a = (char **)ac_malloc(sizeof(char *) * tests);
  for (int i = 0; i < tests; i++) {
    a[i] = ac_malloc(i + 1);
    sleep(speed);
  }
  printf("Cleaning up - intentionally leaving a small leak\n");
  for (int i = 0; i < tests; i++)
    ac_free(a[i]);
  // ac_free(a);
  return 0;
}
