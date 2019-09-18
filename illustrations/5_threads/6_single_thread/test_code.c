#include <stdio.h>

int main(int argc, char *argv[]) {
  int local_number = 0;
  for( int j=0; j<10; j++ ) {
    for( int i=0; i<1000000; i++ )
      local_number++;
  }
  printf( "local_number (should be 10000000)= %d\n", local_number );
  return 0;
}
