#define sort_t size_t
#define print_sort_t(x) printf("%lu ", x)

#define quicksort_type sort_t
#define quicksort_print_type(x) print_sort_t(x)
#define quicksort_name quicksort
// #define quicksort_less(a, b, arg) (a) < (b)
// #define quicksort_arg (if you want to use an argument in less function)
#include "_quicksort.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "timer.h"

int REPEAT_TEST=500;

void run_test(const char *test_name, sort_t *base, size_t num_elements, char *type) {
 // REPEAT_TEST=1;
  printf("%s\n", test_name);
  timer_t *copy_timer = timer_init(REPEAT_TEST);
  timer_start(copy_timer);
  for (int i = 0; i < REPEAT_TEST; i++) {
    memcpy(base + num_elements, base, num_elements * sizeof(sort_t));
  }
  timer_end(copy_timer);

  timer_t *test_timer = timer_timer_init(copy_timer);
  timer_start(test_timer);
  for (int i = 0; i < REPEAT_TEST; i++) {
    memcpy(base + num_elements, base, num_elements * sizeof(sort_t));
    quicksort(base + num_elements, num_elements);
  }
  timer_end(test_timer);

  if (strchr(type, 'P'))
    quicksort_print(__FUNCTION__, __LINE__, base+num_elements, num_elements);
  quicksort_test(__FUNCTION__, __LINE__, base+num_elements, num_elements);

  printf("original: %0.4fmis\n",
         timer_mis(test_timer));

  timer_destroy(test_timer);
  timer_destroy(copy_timer);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
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

  if(num_elements < 100) {
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
  int i;

  for (i = 0; i < num_elements; i++)
    base[i] = i;
  run_test("Ordered test...", base, num_elements, type);

  for (i = 0; i < num_elements; i++)
    base[i] = 100;
  run_test("Equal test...", base, num_elements, type);

  for (i = 0; i < num_elements; i++)
    base[i] = num_elements - i;
  run_test("Reverse test...", base, num_elements, type);

  for (i = 0; i < (num_elements * 6) / 10; i++)
    base[i] = i + 2;
  for (; i < num_elements; i++)
    base[i] = i;
  run_test("Mixed test...", base, num_elements, type);

  for (i = 0; i < (num_elements * 7) / 10; i++)
    base[i] = 1000 + i;
  for (; i < num_elements; i++)
    base[i] = rand();
  run_test("Mixed test2...", base, num_elements, type);

  for (i = 0; i < num_elements / 3; i++)
    base[i] = rand();
  for (; i < num_elements; i++)
    base[i] = i;
  run_test("Mixed test3...", base, num_elements, type);
  for (i = 0; i < num_elements; i++)
    base[i] = rand();
  run_test("Random test...", base, num_elements, type);
  return 0;
}
