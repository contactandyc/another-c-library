#include "file2.c"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main( int argc, char *argv[] ) {
  for( int i=1; i<argc; i++ ) {
    char *s = strdup(argv[i]); // string duplicate
    reverse_string(s, strlen(s));
    printf( "%s => %s\n", argv[i], s );
    free(s);
  }
  return 0;
}
