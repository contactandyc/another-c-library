#include "ac_buffer.h"

void my_buffer_appendf(ac_buffer_t *bh, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ac_buffer_appendvf(bh, fmt, args);
  va_end(args);
}

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setf(bh, "%s ", "Hello");
  my_buffer_appendf(bh, "%s!", "Buffer");

  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
