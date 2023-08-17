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
  /* copy, uppercase, and print command line arguments */
  char **s = ac_strdupan(argv, argc);
  for (int i = 0; i < argc; i++) {
    uppercase(s[i]);
    printf("%s", s[i]);
    if (i + 1 < argc)
      printf(" ");
    else
      printf("\n");
  }
  ac_free(s);
  return 0;
}
