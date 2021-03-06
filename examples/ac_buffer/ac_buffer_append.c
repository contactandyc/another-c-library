#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_sets(bh, "Hello");
  ac_buffer_append(bh, " Buffer!", strlen(" Buffer!")); // append
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
