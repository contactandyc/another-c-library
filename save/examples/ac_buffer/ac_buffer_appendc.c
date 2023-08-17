#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setc(bh, 'H');
  ac_buffer_appendc(bh, 'e'); // appendc
  ac_buffer_appendc(bh, 'l'); // appendc
  ac_buffer_appendc(bh, 'l'); // ...
  ac_buffer_appendc(bh, 'o');
  ac_buffer_appendc(bh, ' ');
  ac_buffer_appendc(bh, 'B');
  ac_buffer_appendc(bh, 'u');
  ac_buffer_appendc(bh, 'f');
  ac_buffer_appendc(bh, 'f');
  ac_buffer_appendc(bh, 'e');
  ac_buffer_appendc(bh, 'r');
  ac_buffer_appendc(bh, '!');
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
