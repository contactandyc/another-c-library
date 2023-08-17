#include "ac_allocator.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  /* Allocate a block that is sized to fit 3 ints */
  int *a = (int *)ac_malloc(sizeof(int) * 3);
  a[0] = 1;
  a[1] = 2;
  a[2] = 3;
  int *b = (int *)ac_memdup(a, sizeof(int) * 3);
  b[0] += 1;
  b[1] += 1;
  b[2] += 1;
  printf("a: %d, %d, %d\n", a[0], a[1], a[2]);
  printf("b: %d, %d, %d\n", b[0], b[1], b[2]);
  /* Free the allocated block */
  ac_free(a);
  ac_free(b);
  return 0;
}
