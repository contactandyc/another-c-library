#include "ac_common.h"
#include "ac_sort.h"
#include "ac_timer.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define sort_t size_t

static inline bool less(const sort_t *a, const sort_t *b) { return *a < *b; }
static void print_sort_t(sort_t *el) { printf("%ld ", *el); }

ac_sort_print_m(quicksort_print, sort_t);
ac_sort_m(quicksort, sort_t, less);
ac_sort_test_m(quicksort_test, less, sort_t);

static inline int compare_size(const void *p1, const void *p2) {
  const sort_t *a = (const sort_t *)p1;
  const sort_t *b = (const sort_t *)p2;
  if (*a != *b)
    return (*a < *b) ? -1 : 1;
  return 0;
}

int REPEAT_TEST = 10000;

void run_test(const char *test_name, sort_t *base, size_t num_elements,
              char *type) {
  // REPEAT_TEST=1;
  printf("%s\n", test_name);
  ac_timer_t *copy_timer = ac_timer_init(REPEAT_TEST);
  ac_timer_start(copy_timer);
  for (int i = 0; i < REPEAT_TEST; i++) {
    memcpy(base + num_elements, base, num_elements * sizeof(sort_t));
  }
  ac_timer_stop(copy_timer);

  ac_timer_t *base_timer = ac_timer_init(REPEAT_TEST);
  ac_timer_subtract(base_timer, copy_timer);
  ac_timer_start(base_timer);
  for (int i = 0; i < REPEAT_TEST; i++) {
    memcpy(base + num_elements, base, num_elements * sizeof(sort_t));
    qsort(base + num_elements, num_elements, sizeof(sort_t), compare_size);
  }
  ac_timer_stop(base_timer);

  ac_timer_t *test_timer = ac_timer_init(REPEAT_TEST);
  ac_timer_subtract(test_timer, copy_timer);
  ac_timer_start(test_timer);
  for (int i = 0; i < REPEAT_TEST; i++) {
    memcpy(base + num_elements, base, num_elements * sizeof(sort_t));
    quicksort(base + num_elements, num_elements);
  }
  ac_timer_stop(test_timer);

  if (strchr(type, 'P'))
    quicksort_print(__AC_FILE_LINE__, base + num_elements, num_elements,
                    print_sort_t);
  quicksort_test(__AC_FILE_LINE__, base + num_elements, num_elements,
                 print_sort_t);

  printf("system qsort: %0.3fns\n", ac_timer_ns(base_timer));
  printf("ac_sort: %0.3fns\n", ac_timer_ns(test_timer));

  ac_timer_destroy(base_timer);
  ac_timer_destroy(test_timer);
  ac_timer_destroy(copy_timer);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("%s <[PA]> <num_elements>\n", argv[0]);
    printf("P - print the results of the sort\n");
    printf("A - do not print the results of the sort\n");

    printf("%s <[PA]> <item1> <item2> ... <itemN>\n", argv[0]);
    printf(
        "same as above, except you provide a list of numbers after type to be "
        "sorted\n");
    return 0;
  }
  char *type = argv[1];
  // srand(time(NULL));
  int num_elements = 0;
  if (sscanf(argv[2], "%d", &num_elements) != 1)
    return -1;

  if (num_elements < 100) {
    REPEAT_TEST = 100000000;
  }

  if (argc > 3) {
    num_elements = argc - 2;
    sort_t *base = (sort_t *)malloc(num_elements * sizeof(sort_t) * 3);
    for (int i = 2; i < argc; i++) {
      if (sscanf(argv[i], "%ld", base + i - 2) != 1)
        abort();
    }
    REPEAT_TEST = 100000000;
    run_test("Manual test...", base, num_elements, type);
    return 0;
  }

  sort_t *base = (sort_t *)malloc((num_elements + 1) * sizeof(sort_t) * 3);
  int i, pos;
  sort_t tmp;

  for (i = 0; i < num_elements; i++)
    base[i] = i;
  run_test("Ordered test...", base, num_elements, type);

  for (i = 0; i < num_elements; i++)
    base[i] = 100;
  run_test("Equal test...", base, num_elements, type);

  for (i = 0; i < num_elements; i++)
    base[i] = num_elements - i;
  run_test("Reverse test...", base, num_elements, type);

  for (i = 0; i < num_elements; i++)
    base[i] = i;
  pos = (num_elements >> 3);
  tmp = base[pos];
  base[pos] = base[pos + 1];
  base[pos + 1] = tmp;
  run_test("Slightly out of ordered test1...", base, num_elements, type);

  for (i = 0; i < num_elements; i++)
    base[i] = i;
  pos = (num_elements >> 3) * 2;
  tmp = base[pos];
  base[pos] = base[pos + 1];
  base[pos + 1] = tmp;
  run_test("Slightly out of ordered test2...", base, num_elements, type);

  for (i = 0; i < num_elements; i++)
    base[i] = rand();
  run_test("Random test...", base, num_elements, type);
  return 0;
}
