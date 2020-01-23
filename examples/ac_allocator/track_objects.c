#include "ac_pool.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Demo to show how objects are tracked\n");
  ac_pool_t *pool = ac_pool_init(1024);
  char *s = ac_pool_strdup(pool, argv[0]);
  // printf("%s\n", s);
  // ac_pool_destroy(pool);
  return 0;
}
