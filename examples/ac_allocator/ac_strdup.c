#include "ac_allocator.h"

#include <stdio.h>

void uppercase(char *s) {
  while(*s) {
    if(*s >= 'a' && *s <= 'z')
      *s = *s - 'a' + 'A';
    s++;
  }
}

int main( int argc, char *argv[]) {
  /* copy, uppercase, and print command line arguments */
  for( int i=0; i<argc; i++ ) {
    char *s = ac_strdup(argv[i]);
    uppercase(s);
    printf("%s", s);
    ac_free(s);
    if(i+1 < argc)
      printf( " " );
    else
      printf( "\n");
  }
  return 0;
}
