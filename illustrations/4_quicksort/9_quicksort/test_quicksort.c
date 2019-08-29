#include <stdio.h>

#define quicksort_type size_t
#define quicksort_name quicksort
// #define quicksort_less(a, b, arg) (a) < (b)
// #define quicksort_arg (if you want to use an argument in less function)
#include "_quicksort.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

size_t *is_list_not_sorted(size_t *p, size_t *lp) {
  p++; // advance to second item
  while (p <= lp) {
    if (p[-1] > *p)
      return p - 1;
    p++;
  }
  return NULL;
}

bool is_list_sorted(size_t *arr, size_t N) { return !is_list_not_sorted(arr, arr+N-1); }

void _print_block(const char *function, int line, size_t *p, size_t *ep) {
  printf("%s:%d (%ld) ", function, line, (ep - p) + 1);
  while (p <= ep) {
    printf("%ld ", *p);
    p++;
  }
  printf("\n");
}

void test_block(size_t *p, ssize_t N) {
  if (!is_list_sorted(p, N)) {
    printf("list is not sorted!\n");
  }
}

#define print_block(a, b) _print_block(__FUNCTION__, __LINE__, a, b)

int REPEAT_TEST=500;

void run_test(const char *test_name, size_t *nums, size_t num, char *type) {
 // REPEAT_TEST=1;
  printf("%s\n", test_name);
  struct timeval t1, t2;
  gettimeofday(&t1, NULL);
  for (int i = 0; i < REPEAT_TEST; i++) {
    memcpy(nums + num, nums, num * sizeof(size_t));
  }
  gettimeofday(&t2, NULL);

  long memcpy_elapsed =
      (t2.tv_sec - t1.tv_sec) * 1000000 + t2.tv_usec - t1.tv_usec;

  gettimeofday(&t1, NULL);
  for (int i = 0; i < REPEAT_TEST; i++) {
    memcpy(nums + num, nums, num * sizeof(size_t));
    quicksort(nums + num, num);
  }
  gettimeofday(&t2, NULL);

  long original_elapsed =
      (t2.tv_sec - t1.tv_sec) * 1000000 + t2.tv_usec - t1.tv_usec;
  original_elapsed -= memcpy_elapsed;

  if (strchr(type, 'P'))
    print_block(nums+num, nums+num+num-1);
  test_block(nums+num, num);

  printf("original: %0.4fns\n",
         original_elapsed / (REPEAT_TEST * 1.0));
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("%s <[PN]> <num>\n", argv[0]);
    printf("P - print the results of the sort\n");
    printf("N - do not print the results of the sort\n");

    printf("%s <[PN]> <item1> <item2> ... <itemN>\n", argv[0]);
    printf(
        "same as above, except you provide a list of numbers after type to be "
        "sorted\n");
    return 0;
  }
  char *type = argv[1];
  // srand(time(NULL));
  int num = 0;
  if (sscanf(argv[2], "%d", &num) != 1)
    return -1;

  if(num < 100) {
    REPEAT_TEST = 100000000;
  }

  if (argc > 3) {
    num = argc - 2;
    size_t *nums = (size_t *)malloc(num * sizeof(size_t) * 3);
    for (int i = 2; i < argc; i++) {
      if (sscanf(argv[i], "%ld", nums + i - 2) != 1)
        abort();
    }
    REPEAT_TEST = 100000000;
    run_test("Manual test...", nums, num, type);
    print_block(nums, nums+num-1);
    return 0;
  }

  size_t *nums = (size_t *)malloc((num + 1) * sizeof(size_t) * 3);
  int i;

  for (i = 0; i < num; i++)
    nums[i] = i;
  run_test("Ordered test...", nums, num, type);

  for (i = 0; i < num; i++)
    nums[i] = 100;
  run_test("Equal test...", nums, num, type);

  for (i = 0; i < num; i++)
    nums[i] = num - i;
  run_test("Reverse test...", nums, num, type);

  for (i = 0; i < (num * 6) / 10; i++)
    nums[i] = i + 2;
  for (; i < num; i++)
    nums[i] = i;
  run_test("Mixed test...", nums, num, type);

  for (i = 0; i < (num * 7) / 10; i++)
    nums[i] = 1000 + i;
  for (; i < num; i++)
    nums[i] = rand();
  run_test("Mixed test2...", nums, num, type);

  for (i = 0; i < num / 3; i++)
    nums[i] = rand();
  for (; i < num; i++)
    nums[i] = i;
  run_test("Mixed test3...", nums, num, type);
  for (i = 0; i < num; i++)
    nums[i] = rand();
  run_test("Random test...", nums, num, type);
  return 0;
}
