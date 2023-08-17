#include "ac_allocator.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  /* Allocate a zeroed block that is sized to fit 3 ints */
  char **s = ac_split(NULL, ',', "A,B,C");
  for (size_t i = 0; s[i] != NULL; i++)
    printf("[%lu] %s\n", i, s[i]);
  ac_free(s);
  return 0;
}
