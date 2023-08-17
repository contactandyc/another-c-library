#include "ac_buffer.h"
#include "ac_common.h"
#include "ac_map.h"
#include "ac_pool.h"
#include "ac_timer.h"

#include <stdio.h>

typedef struct {
  ac_map_t map;
  int born;
  int died;
  char *about;
} name_t;

static inline const char *get_name(const name_t *el) {
  return (char *)(el + 1);
}

static inline int compare_name(const name_t *a, const name_t *b) {
  return strcmp(get_name(a), get_name(b));
}

ac_multimap_insert_m(name_insert, name_t, compare_name);

char *print_name_to_string(ac_pool_t *pool, ac_map_t *n) {
  name_t *el = (name_t *)n;
  return ac_pool_strdupf(pool, "%s", get_name(el));
}

name_t *parse_line(ac_pool_t *pool, char *s) {
  char *name = s;
  while (*s && *s != '\t')
    s++;
  if (!(*s))
    return NULL;
  *s = 0;
  s++;
  char *born = s;
  while (*s && *s != '-')
    s++;
  if (!(*s))
    return NULL;
  *s = 0;
  s++;
  char *died = s;
  while (*s && *s != '\t')
    s++;
  if (!(*s))
    return NULL;
  *s = 0;
  s++;
  char *about = s;
  while (*s && *s != '\r' && *s != '\n')
    s++;
  *s = 0;

  int born_year, died_year;
  if (sscanf(born, "%d", &born_year) != 1 ||
      sscanf(died, "%d", &died_year) != 1)
    return NULL;
  name_t *r = (name_t *)ac_pool_alloc(pool, sizeof(name_t) + strlen(name) + 1);
  strcpy((char *)(r + 1), name);
  r->about = ac_pool_strdup(pool, about);
  r->born = born_year;
  r->died = died_year;
  return r;
}

int main(int argc, char *argv[]) {
  printf("Demo printing red black tree\n");
  ac_pool_t *pool = ac_pool_init(1024);
  ac_map_t *root = NULL;

  int num = 0;
  char str[1000];
  FILE *in = fopen("names.txt", "rb");
  while (fgets(str, 999, in) != NULL) {
    name_t *name = parse_line(pool, str);
    if (!name)
      continue;
    // print_full_name(name);
    name_insert(name, &root);
    num++;
    if (num > 50)
      break;
  }
  fclose(in);

  ac_map_print(pool, root, print_name_to_string, 0);
  ac_pool_destroy(pool);
  return 0;
}
