#include "stla_buffer.h"

int main( int argc, char *argv[] ) {
  stla_buffer_t *bh = stla_buffer_init(10);
  stla_buffer_setc(bh, 'H');
  stla_buffer_appendc(bh, 'e');
  stla_buffer_appendn(bh, 'l', 2); // appendn
  stla_buffer_appendc(bh, 'o');
  stla_buffer_appendc(bh, ' ');
  stla_buffer_appendc(bh, 'B');
  stla_buffer_appendc(bh, 'u');
  stla_buffer_appendn(bh, 'f', 2); // appendn
  stla_buffer_appendc(bh, 'e');
  stla_buffer_appendc(bh, 'r');
  stla_buffer_appendc(bh, '!');
  /* print Hello Buffer! followed with a newline */
  printf( "%s\n", stla_buffer_data(bh) );
  stla_buffer_destroy(bh);
  return 0;
}
