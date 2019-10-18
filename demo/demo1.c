#include "acallocator.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Demo to show how allocations are tracked\n");
  char *s = acstrdup(argv[0]);
  // printf("%s\n", s);
  // acfree(s);
  return 0;
}
