# Is a List or Array Sorted or Not?  Improving Quicksort

# Summary

Determining if a list is sorted is a very basic approach requiring N-1 comparisons of a list which is N items long.

To determine if ABCDE is sorted, you might
```
1.  fail if A is greater than B
2.  fail if B is greater than C
3.  fail if C is greater than D
4.  fail if D is greater than E
5.  list is sorted
```

For a list of 1 million items, it would take at most 999,999 comparison to determine if the list is sorted.  In the example above, BCDEA would be the worst case as it would fail on the last comparison.  In the case of a million items, we could confirm that every 100,000th item is in order before doing the 999,999 comparisons.  The odds of every 100,000th item being in order are 1 in 362,880.  By doing 10 comparisons, 999,999 comparisons can be avoided in determining if a set is sorted in the worst case.

qsort is a standard C function which typically uses a combination of the quicksort algorithm and insertion sort.  The partitioning that is used within qsort is based upon Robert Sedgewick's idea of choosing a median pivot point of the first, middle, and last point.  To find the median, comparisons are made and if they are out of order, swapping occurs.  It is possible to know without any extra comparisons that the first, middle, and last point are in order and didn't need swapping.  Given that this only occurs 1/6th of the time on random sets, it makes sense to continue to see if the full set was fully sorted (or reversed) prior to engaging the quicksort's generally recursive algoithm.  By doing this, the quicksort's worst case scenarios become the best case scenarios!  

I have no idea if this is new or not, but the qsort method in the standard gcc library doesn't apply this optimization.  

# Detailed explanation with C code.

Determining if a list is sorted is a very basic approach requiring N-1 comparisons of a list which is N items long.

In C, we might implement this as follows.
```c
#include <stdbool.h>

bool is_list_sorted(int *arr, size_t N) {
  int *p = arr+1;
  int *ep = arr+N;
  while(p < ep) {
    if(p[-1] > *p)
      return false;
    p++;
  }
  return true;
}
```

Breaking it down...

The function expects a pointer to a series of N integers and will return true if the list is sorted
```c
bool is_list_sorted(int *arr, size_t N) {
  ...
}
```

Set a pointer to the second element
```c
  int *p = arr+1;
```

Set an end pointer to just after the last element
```c
  int *ep = arr+N
```

While the pointer (p) is less than the end pointer (ep)
```c
  while(p < ep) {
    ...
  }
```

If the item prior to the pointer is greater than the current item, the list is not sorted.  *p is the same as p[0].  It gets the value of the item referenced by the pointer.  p[-1] gets the value of the item just before the pointer.  
```c
    if(p[-1] > *p)
      return false;
```

Otherwise, advance the pointer (p) by one.  By returning false if p[-1] is greater than p[0], we are implying that p[-1] <= p[0].
```c
    p++;
```

If you've reached this point, you've checked that every item is less than or equal to the next item.  
```
  return true;
```

What if we wanted to know if a list is not sorted?

The only difference is that true and false are swapped.
```c
#include <stdbool.h>

bool is_list_not_sorted(int *arr, size_t N) {
  int *p = arr+1;
  int *ep = arr+N;
  while(p < ep) {
    if(p[-1] > *p)
      return true;
    p++;
  }
  return false;
}
```

It would be better if we could return a pointer to where the sort failed (or to some point before the failure) and a NULL pointer otherwise.
```c
int *is_list_not_sorted(int *arr, size_t N) {
  int *p = arr+1;
  int *ep = arr+N;
  while(p < ep) {
    if(p[-1] > *p)
      return p;
    p++;
  }
  return NULL;
}
```

Let's check the first and last value before iterating.  The list is not sorted if the first item is greater than last.  This would eliminate the need to iterate 50% of the time.
```c
int *is_list_not_sorted(int *arr, size_t N) {
  if(N < 2) return NULL; // lists of zero or one item are sorted
  int *p = arr; // point to first item
  int *lp = arr+N-1; // changed to lp (to point to the last integer)
  if(*p > *lp)
    return p;
  p++; // advance to second item
  while(p <= lp) { // changed to <= instead of <
    if(p[-1] > *p)
      return p-1;
    p++;
  }
  return NULL;
}
```

To make this clearer, we will break it into two functions.
```c
int is_list_not_sorted_iteration(int *p, int *lp) {
  p++; // advance to second item
  while(p <= lp) {
    if(p[-1] > *p)
      return p-1;
    p++;
  }
  return NULL;  
}

int *is_list_not_sorted(int *arr, size_t N) {
  if(N < 2) return NULL; // lists of zero or one item are sorted
  int *p = arr; // point to first item
  int *lp = arr+N-1; // changed to lp (to point to the last integer)
  if(*p > *lp) // 50% of unsorted cases should pass this test
    return p;
  return is_list_not_sorted_iteration(p, lp);
}
```

Let's check the midpoint.
```c
int *is_list_not_sorted(int *arr, size_t N) {
  if(N < 2) return NULL;
  int *p = arr;
  int *lp = arr+N-1;
  if(*p > *lp)
    return p;
  int *midp = arr + (N>>1);
  if(*p > *midp)
    return p;
  if(*midp > *lp)
    return p;
  // only 16.66% of unsorted cases should arrive here.
  return is_list_not_sorted_iteration(p, lp);
}
```

What if we check 9 points?  First, let's rename our last function for cases where we have 10 or less points.
```c
int *is_list_not_sorted_small(int *arr, size_t N) {
  if(N < 2) return NULL;
  ...
}
```

Then we can modify is_list_not_sorted to check for 9 points.
```c
int *is_list_not_sorted(int *arr, size_t N) {
  if(N < 10)
    return is_list_not_sorted_small(arr, N);

  int *p = arr;
  int *lp = arr+N-1;
  size_t block_size = (N>>3);
  int *np = arr+block_size;
  if(*p > *np)  // test 1
    return p;
  int *np2 = np+block_size;
  if(*np > *np2) // test 2
    return p;
  np = np2+block_size;
  if(*np2 > *np) // test 3
    return p;
  np2 = np+block_size;
  if(*np2 > *np) // test 4
    return p;
  np = np2+block_size;
  if(*np2 > *np) // test 5
    return p;
  np2 = np+block_size;
  if(*np2 > *np) // test 6
    return p;
  np = np2+block_size;
  if(*np2 > *np) // test 7
    return p;
  if(*np > *lp) // test 8
    return p;

  // only 0.0025% of unsorted cases should arrive here.
  return is_list_not_sorted_iteration(p, lp);
}

If the list is unsorted, the odds of it reaching comparison 6 are 1/120.  The odds of it needing to do iteration to determine that the list is unsorted is 1/40230!  Furthermore, if you do reach iteration, you can know how many items were sorted at the front.  I don't believe that anything fancy is needed based upon the number of items in the list.  If our data has any probability of being sorted (which most sets do), then it is extremely unlikely to randomly meet even a 7 point test.

We can now write is_list_sorted differently..

```c
bool is_list_sorted(int *arr, size_t N) {
  return !is_list_not_sorted(arr, N);
}
```

A similar application could apply for lists being completely reversed.
