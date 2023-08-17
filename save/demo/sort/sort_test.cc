#include "ac_allocator.h"
#include "ac_common.h"
#include "ac_sort.h"
#include "ac_timer.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// #include <bits/stdc++.h>
#include <algorithm>

#define sort_t size_t

static inline bool compare(sort_t i, sort_t j) { return i < j; }

void run_test(const char *test_name, sort_t *base, size_t num_elements,
              char order) {
  int repeat = 10000000 / num_elements;
  if (repeat < 100)
    repeat = 100;
  ac_timer_t *copy_timer = ac_timer_init(repeat);
  ac_timer_start(copy_timer);
  for (int i = 0; i < repeat; i++) {
    memcpy(base + num_elements, base, num_elements * sizeof(sort_t));
  }
  ac_timer_stop(copy_timer);

  ac_timer_t *sort_timer = ac_timer_init(repeat);
  ac_timer_subtract(sort_timer, copy_timer);
  ac_timer_start(sort_timer);
  for (int i = 0; i < repeat; i++) {
    memcpy(base + num_elements, base, num_elements * sizeof(sort_t));
    std::sort(base + num_elements, base + num_elements + num_elements, compare);
  }
  ac_timer_stop(sort_timer);

  printf("-%c%s (time in microseconds)\taitems\ta%'9lu\tstd::sort\t%'0.3f\n", order,
         test_name, num_elements, ac_timer_us(sort_timer));

  ac_timer_destroy(sort_timer);
  ac_timer_destroy(copy_timer);
}

int main(int argc, char *argv[]) {
  setlocale(LC_NUMERIC, "");

  sort_t *base = (sort_t *)ac_malloc(1000000 * sizeof(sort_t) * 3);
  int i, pos;
  sort_t tmp;

  size_t num_elements = 10;
  for (int m = 0; m < 3; m++) {
    num_elements = num_elements * num_elements;
    if (num_elements > 1000000)
      num_elements = 1000000;
    for (i = 0; i < num_elements; i++)
      base[i] = i;
    run_test("Ordered test", base, num_elements, 'a');

    for (i = 0; i < num_elements; i++)
      base[i] = 100;
    run_test("Equal test", base, num_elements, 'b');

    for (i = 0; i < num_elements; i++)
      base[i] = num_elements - i;
    run_test("Reverse test", base, num_elements, 'c');

    for (i = 0; i < num_elements; i++)
      base[i] = rand();
    run_test("Random test", base, num_elements, 'd');
  }
  ac_free(base);
  return 0;
}
