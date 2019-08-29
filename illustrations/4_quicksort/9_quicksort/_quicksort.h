#include <stdio.h>

#ifndef quicksort_less
#define quicksort_less(a, b, arg) (a) < (b)
#endif

#define quicksort_swap quicksort_swap##quicksort_name
#define quicksort_stack_t quicksort_stack_t##quicksort_name

static inline void quicksort_swap(quicksort_type *a, quicksort_type *b) {
  quicksort_type tmp = *a;
  *a = *b;
  *b = tmp;
}

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
  quicksort_type *mid, *high;
  ssize_t pivot;
  quicksort_stack_t stack[64];
  quicksort_stack_t *top = stack;
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
  const void **labels = glabels+2;
  void *return_label = NULL;
  top->base = NULL;
  top++;
main_loop:;
  if(num_elements < 10)
    goto *labels[num_elements];
  return_label = &&main_loop_cont;
  goto median_of_5;

main_loop_cont:;
  if(!pivot || quicksort_less(*b, *a, arg) || quicksort_less(*c, *b, arg) || quicksort_less(*d, *c, arg) || quicksort_less(*e, *a, arg)) // test for out of order
    goto median_sort_post_median;
  if(pivot==1) {
    b = a+1;
    while(a < e) {
      if(quicksort_less(*b, *a, arg))
        goto out_of_order;
      a++;
      b++;
    }
    return;
out_of_order:;
    goto fast_sort_post_median;
  }
  else {
    b = e-1;
    while(b > a) {
      if(quicksort_less(*e, *b, arg))
        goto reversed_out_of_order;
      b--;
      e--;
    }
    a = base;
    e = base+num_elements-1;
    while(a<e) {
      quicksort_swap(a,e);
      a++;
      e--;
    }
    return;
reversed_out_of_order:;
    goto fast_sort_post_median;
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
  if(quicksort_less(base[1], base[0], arg))
    quicksort_swap(base, base+1);
  goto sorted;

sort_3:;
  if(quicksort_less(base[1], base[0], arg))
    quicksort_swap(base, base+1);
  if(quicksort_less(base[2], base[1], arg))
    quicksort_swap(base+1,base+2);
  else
    goto sorted;
  if(quicksort_less(base[1], base[0], arg))
    quicksort_swap(base, base+1);
  goto sorted;

sort_4:;
  if(quicksort_less(base[1], base[0], arg))
    quicksort_swap(base, base+1);
  if(quicksort_less(base[3], base[2], arg))
    quicksort_swap(base+2, base+3);
  if(quicksort_less(base[2], base[0], arg)) {
    if(quicksort_less(base[3], base[0], arg)) {
      quicksort_swap(base, base+2);
      quicksort_swap(base+1, base+3);
    }
    else {
      quicksort_swap(base, base+1);
      quicksort_swap(base+0, base+2);
      if(quicksort_less(base[3], base[2], arg))
        quicksort_swap(base+2, base+3);
    }
  }
  else {
    if(quicksort_less(base[2], base[1], arg)) {
      quicksort_swap(base+1, base+2);
      if(quicksort_less(base[3], base[2], arg))
        quicksort_swap(base+2, base+3);
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
  if(quicksort_less(*b, *a, arg))
    quicksort_swap(a, b);
  if(quicksort_less(*e, *d, arg))
    quicksort_swap(d, e);
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
      if(quicksort_less(*e, *d, arg))
        quicksort_swap(c, e);
      else
        quicksort_swap(c, d);
    }
    else if(quicksort_less(*e, *c, arg))
      quicksort_swap(c, e);
  }
  if(quicksort_less(*e, *d, arg))
    quicksort_swap(d, e);
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
      if(quicksort_less(*e, *d, arg))
        quicksort_swap(c, e);
      else
        quicksort_swap(c, d);
    }
    else if(quicksort_less(*e, *c, arg))
      quicksort_swap(c, e);
  }
  if(quicksort_less(*c, base[1], arg)) {
    quicksort_swap(base+1, c);
    if(quicksort_less(*d, *c, arg)) {
      if(quicksort_less(*e, *d, arg))
        quicksort_swap(c, e);
      else
        quicksort_swap(c, d);
    }
    else if(quicksort_less(*e, *c, arg))
      quicksort_swap(c, e);
  }
  if(quicksort_less(*e, *d, arg))
    quicksort_swap(d, e);
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

median_of_5s:;
  if(quicksort_less(*d, *c, arg)) {
    pivot = 0;
    if(quicksort_less(*e, *d, arg))
      quicksort_swap(c, e);
    else
      quicksort_swap(c, d);
  }
  else if(quicksort_less(*e, *c, arg)) {
    pivot = 0;
    quicksort_swap(c, e);
  }
  if(quicksort_less(*c, *a, arg)) {
    pivot = 0;
    quicksort_swap(a, c);
    if(quicksort_less(*d, *c, arg)) {
      if(quicksort_less(*e, *d, arg))
        quicksort_swap(c, e);
      else
        quicksort_swap(c, d);
    }
    else if(quicksort_less(*e, *c, arg))
      quicksort_swap(c, e);
  }
  if(quicksort_less(*c, *b, arg)) {
    pivot = 0;
    quicksort_swap(b, c);
    if(quicksort_less(*d, *c, arg)) {
      if(quicksort_less(*e, *d, arg))
        quicksort_swap(c, e);
      else
        quicksort_swap(c, d);
    }
    else if(quicksort_less(*e, *c, arg))
      quicksort_swap(c, e);
  }
  /* at this point pivot will be 0 if out of order, 1 if inorder (optionally test a==e) */
  goto *return_label;

fast_sort:;
fast_sort_post_median:;
  mid = base + (num_elements>>1);
  high = base + (num_elements-1);
  quicksort_swap(mid, high); // if data is sorted forward or reverse, then mid is best split, if random or equal, it can't hurt
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
  return_label = &&median_sort_cont;
  goto median_of_5;

median_sort_cont:;
  if(!(quicksort_less(*a, *c, arg))) { // likely a bad split
    quicksort_swap(base+1, c);
    goto test_equal;
  }
median_sort_post_median:;
  if(e < a) {
    quicksort_swap(a,e);
    quicksort_swap(b,d);
    mid = a;
    a = e;
    e = mid;
    mid = b;
    b = d;
    d = mid;
  }
  b = a+1;
  d = e-1;
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

#undef quicksort_type
#undef quicksort_name
#undef quicksort_less
#undef quicksort_swap
#undef quicksort_stack_t
#ifdef quicksort_arg
#undef quicksort_arg
#endif
