#include "ac_allocator.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  int days_in_year = 365;
  int months_in_year = 12;
  char *s = ac_strdupf("There are %d days and %d months in a year",
                       days_in_year, months_in_year);
  printf("%s\n", s);
  ac_free(s);
  return 0;
}
