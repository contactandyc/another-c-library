#include "stla_buffer.h"

int main( int argc, char *argv[] ) {
  stla_buffer_t *bh = stla_buffer_init(10);
  stla_buffer_sets(bh, "Hello");
  stla_buffer_appendc(bh, ' ');
  char *data = (char *)stla_buffer_append_alloc(bh, strlen("Buffer!"));
  strcpy(data, "Buffer!");
  /* print Hello Buffer! followed with a newline */
  printf( "%s\n", stla_buffer_data(bh) );
  stla_buffer_destroy(bh);
  return 0;
}
