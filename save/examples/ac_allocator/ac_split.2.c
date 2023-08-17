#include "ac_allocator.h"

#include <stdio.h>

int main( int argc, char *argv[]) {
  /* Allocate a zeroed block that is sized to fit 3 ints */
  size_t num_s = 0;
  char **s = ac_split(&num_s, ',', "A,B,C");
  for( size_t i=0; i<num_s; i++ )
    printf( "[%lu] %s\n", i, s[i]);
  ac_free(s);
  return 0;
}
