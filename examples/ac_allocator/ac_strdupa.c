#include "ac_allocator.h"

#include <stdio.h>

void uppercase(char *s) {
  while (*s) {
    if (*s >= 'a' && *s <= 'z')
      *s = *s - 'a' + 'A';
    s++;
  }
}

int main(int argc, char *argv[]) {
  char **a = ac_split(NULL, ',', "alpha,beta,gamma");
  char **b = ac_strdupa(a);
  for (size_t i = 0; a[i] != NULL; i++) {
    uppercase(b[i]);
    printf("%s=>%s", a[i], b[i]);
    if (a[i + 1] != NULL)
      printf(" ");
    else
      printf("\n");
  }
  ac_free(a);
  ac_free(b);
  return 0;
}
