#include "xxx.h"

int main() {
  xxx_t *handle = xxx_init(1, 2);
  xxx_do_something(handle, "Hello");
  xxx_destroy(handle);
  return 0;
}
