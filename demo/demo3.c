#include "acpool.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Demo to show off pool\n");
  acpool_t *pool = acpool_init(1024);
  char *s = acpool_strdupf(pool, "Hello %s!", argv[0]);
  printf("%s\n", s);
  acpool_destroy(pool);
  return 0;
}
