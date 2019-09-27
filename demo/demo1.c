#include "stla_allocator.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Demo to show how allocations are tracked\n");
  char *s = stla_strdup(argv[0]);
  // printf("%s\n", s);
  // stla_free(s);
  return 0;
}
