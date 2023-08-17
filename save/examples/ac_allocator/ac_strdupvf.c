#include "ac_allocator.h"

#include <stdio.h>

char *my_strdupf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char *r = ac_strdupvf(fmt, args);
  va_end(args);
  return r;
}

int main(int argc, char *argv[]) {
  int days_in_year = 365;
  int months_in_year = 12;
  char *s = my_strdupf("There are %d days and %d months in a year",
                       days_in_year, months_in_year);
  printf("%s\n", s);
  ac_free(s);
  return 0;
}
