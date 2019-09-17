#include "stla_buffer.h"

int main( int argc, char *argv[] ) {
  stla_buffer_t *bh = stla_buffer_init(10);
  stla_buffer_set(bh, "Hello Buffer!", strlen("Hello Buffer!")); // set
  /* print Hello Buffer! followed with a newline */
  printf( "%s\n", stla_buffer_data(bh) );
  stla_buffer_destroy(bh);
  return 0;
}
