#include "ac_allocator.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Demo to show how allocations are tracked\n");
  char *s = ac_strdup(argv[0]);
  printf("%s\n", s);
  // ac_free(s);
  return 0;
}
