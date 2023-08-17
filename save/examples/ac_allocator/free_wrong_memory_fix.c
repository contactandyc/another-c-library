#include "ac_allocator.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Demo to show invalid free\n");
  char *s = ac_strdup(argv[0]);
  // s += 2; // FIX WAS HERE!
  ac_free(s);
  return 0;
}
