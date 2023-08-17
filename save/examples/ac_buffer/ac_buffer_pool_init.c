#include "ac_buffer.h"
#include "ac_pool.h"

int main(int argc, char *argv[]) {
  ac_pool_t *pool = ac_pool_init(4096);
  ac_buffer_t *bh = ac_buffer_pool_init(pool, 10);
  ac_buffer_setf(bh, "%s %s!", "Hello", "Buffer"); // setf
  /* print Hello Buffer! followed with a newline */
  printf("%s\n", ac_buffer_data(bh));
  // Not needed because pool used!
  // ac_buffer_destroy(bh);
  ac_pool_destroy(pool);
  return 0;
}
