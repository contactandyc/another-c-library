#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setf(bh, "%s %s!", "Hello", "Buffer"); // setf
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_clear(bh);
  // notice appendf instead of setf... since after clear it will be equivalent
  ac_buffer_appendf(bh, "%s %s!", "Hello", "Again"); // setf
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
