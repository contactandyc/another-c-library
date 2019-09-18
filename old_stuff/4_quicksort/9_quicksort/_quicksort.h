#include <stdio.h>
#include <stdlib.h>

#ifndef quicksort_less
#define quicksort_less(a, b, arg) (a) < (b)
#endif

/*
  In order for gcc to concatenate a predefined variable, it must be nested
  like the following.

  quicksort_concat2(quicksort_name, sort) => quicksort_name_sort

  instead of the value of quicksort_name followed by _sort.
*/
#define quicksort_concat2(x,y) x ## _ ## y
#define quicksort_concat1(x, y) quicksort_concat2(x, y)
#define quicksort_concat(y) quicksort_concat1(quicksort_name, y)

#define quicksort_stack_t quicksort_concat(stack_t)
#define quicksort_sorted quicksort_concat(sorted)
#define quicksort_test quicksort_concat(test)

#ifdef quicksort_print_type
#define quicksort_print quicksort_concat(print)
void quicksort_print(const char *function, int line, quicksort_type *base, ssize_t num_elements) {
  printf("%s:%d %ld elements\n", function, line, num_elements);
  quicksort_type *ep = base+num_elements;
  while (base < ep) {
    quicksort_print_type(*base);
    base++;
  }
  printf("\n");
}
#endif

int quicksort_sorted(quicksort_type *base, ssize_t num_elements) {
  quicksort_type *ep = base+num_elements;
  base++;
  while (base < ep) {
    if (base[-1] > *base)
      return 0;
    base++;
  }
  return 1;
}

void quicksort_test(const char *function, int line, quicksort_type *base, ssize_t num_elements) {
  if (!quicksort_sorted(base, num_elements)) {
    printf("%s:%d (%ld) list is not sorted!\n", function, line, num_elements);
#ifdef quicksort_print
    quicksort_print(function, line, base, num_elements);
#endif
    abort();
  }
}

#define quicksort_swap(a, b) \
  tmp = *(a); \
  *(a) = *(b); \
  *(b) = tmp

typedef struct {
  quicksort_type *base;
  ssize_t num_elements;
} quicksort_stack_t;

#ifdef quicksort_arg
void quicksort_name(quicksort_type *base, ssize_t num_elements, void *arg) {
#else
void quicksort_name(quicksort_type *base, ssize_t num_elements) {
#endif
  quicksort_type *a, *b, *c, *d, *e;
  quicksort_type *mid, *high, tmp;
  ssize_t pivot;
  quicksort_stack_t stack[64];
  const void *glabels[43] = { &&sorted, &&sorted,
    &&sorted, &&sorted, &&sort_2, &&sort_3, &&sort_4,
    &&sort_5, &&sort_6, &&sort_7, &&fast_sort, &&fast_sort,
    &&fast_sort, &&fast_sort, &&fast_sort, &&fast_sort, &&fast_sort,
    &&fast_sort, &&fast_sort, &&fast_sort, &&fast_sort, &&fast_sort,
    &&fast_sort, &&fast_sort, &&fast_sort, &&fast_sort, &&fast_sort,
    &&fast_sort, &&fast_sort, &&fast_sort, &&fast_sort, &&fast_sort,
    &&fast_sort, &&fast_sort, &&fast_sort, &&fast_sort, &&fast_sort,
    &&fast_sort, &&fast_sort, &&fast_sort, &&fast_sort, &&fast_sort,
    &&median_sort };
  void *return_label = NULL;
  const void **labels = glabels+2;
  quicksort_stack_t *top = stack;
  top->base = NULL;
  top++;
  if(num_elements < 12)
    goto *labels[num_elements];

  a = base;
  e = base+num_elements-1;
  pivot = num_elements>>1;
  c = a+pivot;
  if(quicksort_less(*e, *a, arg)) {
    b = a;
    a = e;
    e = b;
    pivot >>= 1;
    b = c+pivot;
    d = e+pivot;
    pivot = -1;
  }
  else {
    pivot >>= 1;
    b = a+pivot;
    d = c+pivot;
    pivot = 1;
  }
  return_label = &&main_loop_cont;
  goto median_of_5s;

main_loop_cont:;
  if(!pivot || quicksort_less(*b, *a, arg) || quicksort_less(*c, *b, arg) || quicksort_less(*d, *c, arg) || quicksort_less(*e, *a, arg)) {
    // test for out of order
    if(e < a) {
      mid = a;
      a = e;
      e = mid;
      mid = b;
      b = d;
      d = mid;
      quicksort_swap(a,e);
      quicksort_swap(b,d);
    }
    goto median_sort_post_median;
  }
  if(pivot==1) {
    mid = a+1;
    while(a < e) {
      if(quicksort_less(*mid, *a, arg))
        goto out_of_order;
      a++;
      mid++;
    }
    return;
out_of_order:;
    a = base;
    goto median_sort_post_median;
  }
  else {
    if(e < a) {
      mid = a;
      a = e;
      e = mid;
      mid = b;
      b = d;
      d = mid;
    }
    mid = e-1;
    high = e;
    while(mid > a) {
      if(quicksort_less(*mid, *high, arg))
        goto reversed_out_of_order;
      high--;
      mid--;
    }
    while(a<e) {
      quicksort_swap(a,e);
      a++;
      e--;
    }
    return;
reversed_out_of_order:;
    quicksort_swap(a,e);
    quicksort_swap(b,d);
    goto median_sort_post_median;
  }

  while(1) {
loop:;
    if(num_elements < 40)
      goto *labels[num_elements];
    else
      goto *labels[40];
sorted:;
    top--;
    base = top->base;
    if(!base)
      return;
    num_elements = top->num_elements;
  }

sort_2:;
  if(quicksort_less(base[1], base[0], arg)) {
    quicksort_swap(base, base+1);
  }
  goto sorted;

sort_3:;
  if(quicksort_less(base[1], base[0], arg)) {
    quicksort_swap(base, base+1);
  }
  if(quicksort_less(base[2], base[1], arg)) {
    quicksort_swap(base+1,base+2);
  }
  else
    goto sorted;
  if(quicksort_less(base[1], base[0], arg)) {
    quicksort_swap(base, base+1);
  }
  goto sorted;

sort_4:;
  if(quicksort_less(base[1], base[0], arg)) {
    quicksort_swap(base, base+1);
  }
  if(quicksort_less(base[3], base[2], arg)) {
    quicksort_swap(base+2, base+3);
  }
  if(quicksort_less(base[2], base[0], arg)) {
    if(quicksort_less(base[3], base[0], arg)) {
      quicksort_swap(base, base+2);
      quicksort_swap(base+1, base+3);
    }
    else {
      quicksort_swap(base, base+1);
      quicksort_swap(base+0, base+2);
      if(quicksort_less(base[3], base[2], arg)) {
        quicksort_swap(base+2, base+3);
      }
    }
  }
  else {
    if(quicksort_less(base[2], base[1], arg)) {
      quicksort_swap(base+1, base+2);
      if(quicksort_less(base[3], base[2], arg)) {
        quicksort_swap(base+2, base+3);
      }
    }
  }
  goto sorted;

sort_5:;
  a = base;
  b = a+1;
  c = b+1;
  d = c+1;
  e = d+1;
  return_label = &&sort_5cont;
  goto median_of_5s;
sort_5cont:;
  if(quicksort_less(*b, *a, arg)) {
    quicksort_swap(a, b);
  }
  if(quicksort_less(*e, *d, arg)) {
    quicksort_swap(d, e);
  }
  goto sorted;

sort_6:;
  a = base+1;
  b = a+1;
  c = b+1;
  d = c+1;
  e = d+1;
  return_label = &&sort_6cont;
  goto median_of_5s;
sort_6cont:;
  if(quicksort_less(*c, *base, arg)) {
    quicksort_swap(base, c);
    if(quicksort_less(*d, *c, arg)) {
      if(quicksort_less(*e, *d, arg)) {
        quicksort_swap(c, e);
      }
      else {
        quicksort_swap(c, d);
      }
    }
    else if(quicksort_less(*e, *c, arg)) {
      quicksort_swap(c, e);
    }
  }
  if(quicksort_less(*e, *d, arg)) {
    quicksort_swap(d, e);
  }
  goto sort_3;

sort_7:;
  a = base+2;
  b = a+1;
  c = b+1;
  d = c+1;
  e = d+1;
  return_label = &&sort_7cont;
  goto median_of_5s;
sort_7cont:;
  if(quicksort_less(*c, *base, arg)) {
    quicksort_swap(base, c);
    if(quicksort_less(*d, *c, arg)) {
      if(quicksort_less(*e, *d, arg)) {
        quicksort_swap(c, e);
      }
      else {
        quicksort_swap(c, d);
      }
    }
    else if(quicksort_less(*e, *c, arg)) {
      quicksort_swap(c, e);
    }
  }
  if(quicksort_less(*c, base[1], arg)) {
    quicksort_swap(base+1, c);
    if(quicksort_less(*d, *c, arg)) {
      if(quicksort_less(*e, *d, arg)) {
        quicksort_swap(c, e);
      }
      else {
        quicksort_swap(c, d);
      }
    }
    else if(quicksort_less(*e, *c, arg)) {
      quicksort_swap(c, e);
    }
  }
  if(quicksort_less(*e, *d, arg)) {
    quicksort_swap(d, e);
  }
  goto sort_4;


test_equal:;
  a = base+2;
  while(a<=high && !(quicksort_less(*base, *a, arg)))
    a++;
  num_elements -= (a-base);
  base = a;
  goto loop;


median_of_5:;
  a = base;
  e = base+num_elements-1;
  pivot = num_elements>>1;
  c = a+pivot;
  pivot >>= 1;
  b = a+pivot;
  d = c+pivot;
  pivot = 1;

median_of_5s:;
  if(quicksort_less(*d, *c, arg)) {
    pivot = 0;
    if(quicksort_less(*e, *d, arg)) {
      quicksort_swap(c, e);
    }
    else {
      quicksort_swap(c, d);
    }
  }
  else if(quicksort_less(*e, *c, arg)) {
    pivot = 0;
    quicksort_swap(c, e);
  }
  if(quicksort_less(*c, *a, arg)) {
    pivot = 0;
    quicksort_swap(a, c);
    if(quicksort_less(*d, *c, arg)) {
      if(quicksort_less(*e, *d, arg)) {
        quicksort_swap(c, e);
      }
      else {
        quicksort_swap(c, d);
      }
    }
    else if(quicksort_less(*e, *c, arg)) {
      quicksort_swap(c, e);
    }
  }
  if(quicksort_less(*c, *b, arg)) {
    pivot = 0;
    quicksort_swap(b, c);
    if(quicksort_less(*d, *c, arg)) {
      if(quicksort_less(*e, *d, arg)) {
        quicksort_swap(c, e);
      }
      else {
        quicksort_swap(c, d);
      }
    }
    else if(quicksort_less(*e, *c, arg)) {
      quicksort_swap(c, e);
    }
  }
  /* at this point pivot will be 0 if out of order, 1 if inorder (optionally test a==e) */
  goto *return_label;

fast_sort:;
  mid = base + (num_elements>>1);
  high = base + (num_elements-1);
  if(quicksort_less(*mid, *base, arg)) {
    quicksort_swap(base, mid);
  }
  if(quicksort_less(*high, *mid, arg)) {
    if(quicksort_less(*high, *base, arg)) {
      quicksort_swap(base,high);
    }
  }
  else {
    quicksort_swap(mid, high);
  }
  a = b = base;
  while(a < high) {
    if(quicksort_less(*a, *high, arg)) {
      quicksort_swap(a, b);
      b++;
    }
    a++;
  }
  quicksort_swap(b, high);
  pivot = b-base;

split:;
  if((pivot<<1) < num_elements) {
    if(pivot == 0 && !(quicksort_less(*base, base[1], arg)))
      goto test_equal;
    top->base=base+pivot+1;
    top->num_elements = num_elements-(pivot+1);
    num_elements = pivot;
  }
  else {
    top->base = base;
    top->num_elements = pivot;
    base += pivot+1;
    num_elements -= (pivot+1);
  }
  top++;
  if(num_elements < 40)
    goto *labels[num_elements];
  else
    goto *labels[40];
  // goto loop;

median_sort:;
  return_label = &&median_sort_post_median;
  goto median_of_5;

median_sort_post_median:;
  quicksort_swap(b,a+1);
  quicksort_swap(d,e-1);
  b = a+2;
  d = e-2;
  e = c;
  while(1) {
    while(quicksort_less(*b, *c, arg))
      b++;
    while(quicksort_less(*c, *d, arg))
      d--;
    if(b<d) {
      quicksort_swap(b, d);
      if(b==c)
        c = d;
      else if(d==c)
        c = b;
      b++;
      d--;
    }
    else
      break;
  }
  if(e != c) {
    if(c <= d) {
      quicksort_swap(c, d);
      pivot = d-a;
    }
    else {
      quicksort_swap(c, b);
      pivot = b-a;
    }
  }
  else {
    pivot = c-a;
  }
  goto split;
}

#ifdef quicksort_type_print
#undef quicksort_type_print
#endif
#ifdef quicksort_print
#undef quicksort_print
#endif
#undef quicksort_concat2
#undef quicksort_concat1
#undef quicksort_concat
#undef quicksort_sorted
#undef quicksort_test
#undef quicksort_type
#undef quicksort_name
#undef quicksort_less
#undef quicksort_swap
#undef quicksort_stack_t
#ifdef quicksort_arg
#undef quicksort_arg
#endif
