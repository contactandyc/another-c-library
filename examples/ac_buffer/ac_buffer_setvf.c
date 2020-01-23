#include "ac_buffer.h"

void my_buffer_setf(ac_buffer_t *bh, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ac_buffer_setvf(bh, fmt, args);
  va_end(args);
}

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  my_buffer_setf(bh, "%s %s!", "Hello", "Buffer"); // setf
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
