#include "stla_buffer.h"

int main( int argc, char *argv[] ) {
  stla_buffer_t *bh = stla_buffer_init(10);
  char *data = (char *)stla_buffer_alloc(bh, strlen("Hello Buffer!"));
  strcpy(data, "Hello Buffer!");
  /* print Hello Buffer! followed with a newline */
  printf( "%s\n", stla_buffer_data(bh) );
  stla_buffer_destroy(bh);
  return 0;
}
