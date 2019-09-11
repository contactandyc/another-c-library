#include "xxx.h"

#include <stdio.h>
#include <stdlib.h>

struct xxx_s {
  int x;
  int y;
};

xxx_t * xxx_init(int param1, int param2) {
  xxx_t *h = (xxx_t *)malloc(sizeof(xxx_t));
  h->x = param1;
  h->y = param2;
  return h;
}

void xxx_destroy( xxx_t *h ) {
  free(h);
}

void xxx_do_something( xxx_t *h, const char *prefix ) {
  printf( "%s: (%d, %d)\n", prefix, h->x, h->y );
}
