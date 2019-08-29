#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(char *a, char *b) {
  char tmp = *a;
  *a = *b;
  *b = tmp;
}

int find_pivot(char *low, char *high) {
  char *wp = low;
  char *p = low;
  while(p < high) {
    if(*p < *high) {
      swap(p, wp);
      wp++;
    }
    p++;
  }
  swap(wp, high);
  return wp-low;
}

void quicksort(char *low, char *high) {
  if(low < high) {
    int pivot = find_pivot(low, high);
    quicksort(low, low+pivot-1);
    quicksort(low+pivot+1, high);
  }
}

int main( int argc, char *argv[]) {
  for( int i=1; i<argc; i++ ) {
    char *s = strdup(argv[i]);
    quicksort(s, s+strlen(s)-1);
    printf("%s => %s\n", argv[i], s);
    free(s);
  }
  return 0;
}
