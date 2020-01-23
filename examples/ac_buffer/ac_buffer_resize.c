#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_sets(bh, "Hello Buffer!"); // sets
  ac_buffer_resize(bh, 5);
  /* print Hello followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
