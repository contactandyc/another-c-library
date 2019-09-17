#include "buffer.h"

int main( int argc, char *argv[] ) {
  buffer_t *bh = buffer_init(10);
  buffer_sets(bh, "Hello");
  buffer_appendc(bh, ' ');
  buffer_appendf(bh, "%s!", "Buffer");
  /* print Hello Buffer! followed with a newline */
  printf( "%s\n", buffer_data(bh) );
  buffer_destroy(bh);
  return 0;
}
