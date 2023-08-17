#include "ac_buffer.h"

int main(int argc, char *argv[]) {
  ac_buffer_t *bh = ac_buffer_init(10);
  char *data = (char *)ac_buffer_alloc(bh, strlen("Hello Buffer!"));
  strcpy(data, "Hello Buffer!");
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return 0;
}
