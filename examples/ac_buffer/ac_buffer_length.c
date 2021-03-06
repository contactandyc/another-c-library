#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setf(bh, "%s %s!", "Hello", "Buffer"); // setf
  /* print Hello Buffer! followed with a newline */
  printf("%lu: %s\n", ac_buffer_length(bh), ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
