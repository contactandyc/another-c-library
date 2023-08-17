#include "ac_allocator.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  /* Allocate a zeroed block that is sized to fit 3 ints */
  int *x = (int *)ac_malloc(sizeof(int) * 2);
  x[0] = 1;
  x[1] = 2;
  x = (int *)ac_realloc(x, sizeof(int) * 3);
  x[2] = 3;

  /* This should print 1, 2, 3 */
  printf("%d, %d, %d\n", x[0], x[1], x[2]);
  /* Free the allocated block */
  ac_free(x);
  return 0;
}
