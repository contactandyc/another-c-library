#include "stla_buffer.h"
#include "stla_common.h"
#include "stla_map.h"
#include "stla_pool.h"
#include "stla_search.h"
#include "stla_sort.h"
#include "stla_timer.h"

#include <stdio.h>

typedef struct {
  stla_map_t map;
  int born;
  int died;
  char *about;
} name_t;

static inline const char *get_name(const name_t *el) {
  return (char *)(el + 1);
}

void print_full_name(const name_t *el) {
  printf("%s\t%d\t%d\t%s\n", get_name(el), el->born, el->died, el->about);
}

void print_name_only(const name_t *el) { printf("%s", get_name(el)); }

static inline int compare_name(const name_t *a, const name_t *b) {
  return strcmp(get_name(a), get_name(b));
}

stla_multimap_insert_m(name_insert, name_t, compare_name);

static inline int compare_name_for_find(const char *a, const name_t *b) {
  return strcmp(a, get_name(b));
}

stla_map_find_m(name_find, char, name_t, compare_name_for_find);
stla_map_least_m(name_least, char, name_t, compare_name_for_find);
stla_map_greatest_m(name_greatest, char, name_t, compare_name_for_find);

static inline int compare_name_partial(const char *a, const name_t *b) {
  return strncmp(a, get_name(b), strlen(a));
}

stla_map_lower_bound_m(name_lower_bound, char, name_t, compare_name_partial);
stla_map_upper_bound_m(name_upper_bound, char, name_t, compare_name_partial);

static inline bool less_name(name_t **a, name_t **b) {
  return strcmp(get_name(*a), get_name(*b)) < 0 ? true : false;
}

stla_sort_m(name_sort, name_t *, less_name);
/*
You can alternatively use a macro for stla_sort_m if needed
#define less_name_defined(a, b) (strcmp(get_name(*a), get_name(*b)) < 0)

stla_sort_m(name_sort, name_t *, less_name_defined);
*/

static inline bool reverse_less_name(name_t **a, name_t **b) {
  return strcmp(get_name(*a), get_name(*b)) > 0 ? true : false;
}

stla_sort_m(name_reverse, name_t *, reverse_less_name);

static inline int compare_name_for_search(char *a, name_t **b) {
  return strcmp(a, get_name(*b));
}

static inline int compare_partial_name_for_search(char *a, name_t **b) {
  return strncmp(a, get_name(*b), strlen(a));
}

stla_search_m(name_search, char, name_t *, compare_name_for_search);
stla_search_least_m(name_search_least, char, name_t *, compare_name_for_search);
stla_search_greatest_m(name_search_greatest, char, name_t *,
                       compare_name_for_search);
stla_search_lower_bound_m(name_search_lower_bound, char, name_t *,
                          compare_partial_name_for_search);
stla_search_upper_bound_m(name_search_upper_bound, char, name_t *,
                          compare_partial_name_for_search);

int compare_name_for_qsort(const void *p1, const void *p2) {
  name_t **a = (name_t **)p1;
  name_t **b = (name_t **)p2;
  return strcmp(get_name(*a), get_name(*b));
}

int reverse_compare_name_for_qsort(const void *p1, const void *p2) {
  name_t **a = (name_t **)p1;
  name_t **b = (name_t **)p2;
  return -strcmp(get_name(*a), get_name(*b));
}

name_t *parse_line(stla_pool_t *pool, char *s) {
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
  name_t *r =
      (name_t *)stla_pool_alloc(pool, sizeof(name_t) + strlen(name) + 1);
  strcpy((char *)(r + 1), name);
  r->about = stla_pool_strdup(pool, about);
  r->born = born_year;
  r->died = died_year;
  return r;
}

void print_search_context(name_t **n, name_t **base, size_t num_elements) {
  print_full_name(*n);
  if (n + 1 < base + num_elements) {
    printf("next: ");
    print_full_name(n[1]);
  }
  if (n - 1 >= base) {
    printf("previous: ");
    print_full_name(n[-1]);
  }
  printf("\n");
}

void print_context(name_t *n) {
  print_full_name(n);
  printf("next: ");
  name_t *n2 = (name_t *)stla_map_next((stla_map_t *)n);
  if (n2)
    print_full_name(n2);
  else
    printf("\n");
  printf("previous: ");
  n2 = (name_t *)stla_map_previous((stla_map_t *)n);
  if (n2)
    print_full_name(n2);
  else
    printf("\n");
  printf("\n");
}

int main(int argc, char *argv[]) {
  printf("Demo to show off different objects\n");
  stla_pool_t *pool = stla_pool_init(1024);
  stla_buffer_t *bh = stla_buffer_init(1024);
  stla_map_t *root = NULL;

  char str[1000];
  FILE *in = fopen("names.txt", "rb");
  while (fgets(str, 999, in) != NULL) {
    name_t *name = parse_line(pool, str);
    if (!name)
      continue;
    // print_full_name(name);
    name_insert(name, &root);
    stla_buffer_append(bh, &name, sizeof(name));
  }
  fclose(in);

  name_t **names = (name_t **)stla_buffer_data(bh);
  size_t num_names = stla_buffer_length(bh) / sizeof(name_t *);

  int repeat = 1000;
  name_t **names_backup =
      (name_t **)stla_pool_dup(pool, names, num_names * sizeof(name_t *));
  stla_timer_t *c = stla_timer_init(repeat);
  stla_timer_start(c);
  for (int i = 0; i < repeat; i++) {
    memcpy(names, names_backup, num_names * sizeof(name_t *));
  }
  stla_timer_stop(c);

  printf("Timing sort algorithms\n");
  stla_timer_t *t = stla_timer_init(repeat);
  stla_timer_subtract(t, c);
  stla_timer_start(t);
  for (int i = 0; i < repeat; i++) {
    memcpy(names, names_backup, num_names * sizeof(name_t *));
    name_sort(names, num_names);
    // name_reverse(names, num_names);
  }
  stla_timer_stop(t);
  printf("sort for stla sort = %0.5f nanoseconds\n", stla_timer_ns(t));
  stla_timer_destroy(t);

  t = stla_timer_init(repeat);
  stla_timer_subtract(t, c);
  stla_timer_start(t);
  for (int i = 0; i < repeat; i++) {
    memcpy(names, names_backup, num_names * sizeof(name_t *));
    // qsort(names, num_names, sizeof(name_t *),
    // reverse_compare_name_for_qsort);
    qsort(names, num_names, sizeof(name_t *), compare_name_for_qsort);
  }
  stla_timer_stop(t);
  printf("sort for regular qsort = %0.5f nanoseconds\n", stla_timer_ns(t));
  stla_timer_destroy(t);

  t = stla_timer_init(repeat);
  stla_timer_start(t);
  for (int i = 0; i < repeat; i++) {
    name_sort(names, num_names);
  }
  stla_timer_stop(t);
  printf("sort (already sorted) for stla sort = %0.5f nanoseconds\n",
         stla_timer_ns(t));
  stla_timer_destroy(t);

  t = stla_timer_init(repeat);
  stla_timer_start(t);
  for (int i = 0; i < repeat; i++) {
    qsort(names, num_names, sizeof(name_t *), compare_name_for_qsort);
  }
  stla_timer_stop(t);
  printf("sort (already sorted) for regular qsort = %0.5f nanoseconds\n",
         stla_timer_ns(t));
  stla_timer_destroy(t);

  name_reverse(names, num_names);
  memcpy(names_backup, names, num_names * sizeof(name_t *));

  t = stla_timer_init(repeat);
  stla_timer_subtract(t, c);
  stla_timer_start(t);
  for (int i = 0; i < repeat; i++) {
    memcpy(names, names_backup, num_names * sizeof(name_t *));
    name_reverse(names, num_names);
  }
  stla_timer_stop(t);
  printf("sort (reversed) for stla sort = %0.5f nanoseconds\n",
         stla_timer_ns(t));
  stla_timer_destroy(t);

  t = stla_timer_init(repeat);
  stla_timer_subtract(t, c);
  stla_timer_start(t);
  for (int i = 0; i < repeat; i++) {
    memcpy(names, names_backup, num_names * sizeof(name_t *));
    qsort(names, num_names, sizeof(name_t *), reverse_compare_name_for_qsort);
  }
  stla_timer_stop(t);
  printf("sort (reversed) for regular qsort = %0.5f nanoseconds\n",
         stla_timer_ns(t));
  stla_timer_destroy(t);
  stla_timer_destroy(c);

  name_sort(names, num_names);

  printf("\nShowing up to 15 rows of sorted output\n");
  for (size_t i = 0; i < num_names && i < 15; i++) {
    print_full_name(names[i]);
  }
  printf("\n");

  for (int i = 1; i < argc; i++) {
    printf("stla_search_m: %s\n", argv[i]);
    name_t **n = name_search(argv[i], names, num_names);
    if (n)
      print_search_context(n, names, num_names);

    printf("stla_search_least_m: %s\n", argv[i]);
    n = name_search_least(argv[i], names, num_names);
    if (n)
      print_search_context(n, names, num_names);

    printf("stla_search_greatest_m: %s\n", argv[i]);
    n = name_search_greatest(argv[i], names, num_names);
    if (n)
      print_search_context(n, names, num_names);

    printf("stla_search_lower_bound_m: %s\n", argv[i]);
    n = name_search_lower_bound(argv[i], names, num_names);
    if (n)
      print_search_context(n, names, num_names);

    printf("stla_search_upper_bound_m: %s\n", argv[i]);
    n = name_search_upper_bound(argv[i], names, num_names);
    if (n)
      print_search_context(n, names, num_names);
  }

  for (int i = 1; i < argc; i++) {
    printf("stla_map_find_m: %s\n", argv[i]);
    name_t *n = name_find(argv[i], root);
    if (n)
      print_context(n);

    printf("stla_map_least_m: %s\n", argv[i]);
    n = name_least(argv[i], root);
    if (n)
      print_context(n);

    printf("stla_map_greatest_m: %s\n", argv[i]);
    n = name_greatest(argv[i], root);
    if (n)
      print_context(n);

    printf("stla_map_lower_bound_m: %s\n", argv[i]);
    n = name_lower_bound(argv[i], root);
    if (n)
      print_context(n);

    printf("stla_map_upper_bound_m: %s\n", argv[i]);
    n = name_upper_bound(argv[i], root);
    if (n)
      print_context(n);
  }
  stla_buffer_destroy(bh);
  stla_pool_destroy(pool);
  return 0;
}
