# Quicksort

Quicksort is an extremely popular sorting algorithm developed by Tony Hoare.  His basic intuition was that sorting a set of items could be done using a sequence of swaps around a pivot point.  The pivot point would be used to divide the data such that all data less than or equal to the pivot would sit on one side and the rest of data would sit on the other side.  

For example, if E was the pivot below, the data would be transformed from...

```
AFDBDKJE
```

To

```
ADBDEFKJ
    ^
```

and the pivot mark would be at the E character.  The pivot point breaks the set into two pieces.

```
ADBDE FKJ
```

Technically, the set can be broken into three pieces since E is guaranteed to be greater than or equal to everything to the left of E and less than or equal to everything on the right of it.

```
ADBD E FKJ
```

The problem then becomes a recursive problem and the left and right sets are further split.  

If we choose the last element to be the pivot
```
ADBD
   ^
```

and we use <= pivot to do split, then our result is
```
ADB D
    ^
```

We now have 4 partitions
```
ADB D E FKJ
```

If we then partition choosing the last element
```
ADB
  ^
```

and we use <= pivot to do split, then our result is
```
A B D
  ^
```

All partitions look like
```
A B D D E FKJ
```

The only partition that remains is FKJ
```
FKJ
  ^
```

becomes
```
F J K
  ^
```

All partitions look like
```
A B D D E F J K
```

Our data is sorted!

Tony Hoare, Nico Lomuto, and Robert Sedgewick proposed various methods of pivoting including choosing the first, last, random, and a median of three based upon the first, middle, and last item in a set.  In addition to this, considering when to use an alternate algorithm, has been an area of discovery over time.  If you always pivot on the last item of a sorted set, then you will have the worst case scenario.  If the set has N items, there there would be N splits.  Each split would require a scan of N items (decreasing by 1 for each successive split).  In addition to this, it is ideal for splits to produce three sets (left, pivot, and right).  The sorted set would continuously produce two sets (left and pivot).

A small example below illustrates how using quicksort and a sorted set provides a worst case scenario using the last item and <= pivot.
````
ABCDE => ABCD E => ABC D E => AB C D E => A B C D E
````

If the data was randomized, there is one less split
````
BDEAC => AB C DE => A B C DE => A B C D E
````

Using the median of three (when there are 3 more or more elements)
```
ABCDE (median of A, C, and E is C)
  ^

AB C DE
   ^

A B C DE

A B C D E
```

Median of 3 for BDEAC becomes same as choosing last element in this case, except when split is done, there is 2 on either side (BA and ED)
```
BDEAC (median of B, E, and C is C)
    ^
```

If in the process of choosing the median, one also swaps the 3 elements in order, then the split is better.

```
BDEAC choose B, E, and C (first, middle, and last points)
^ ^ ^
```

Sort B, E, and C and choose C
```
BDCAE
  ^
```

Imagine two additional pointers (left (L) and right (R) which initially point to the first and last element).  We will also label the middle point M.
```
BDCAE
L M R
```

We can advance L while it is less than M (B is less than C, D is not less than C), so stop L at D
We can advance R while it is greater than M (E is greater than C, A is not greater than C), so stop R at A
```
BDCAE
 LMR
```

At this point, L and R point to items which are on the wrong side of the pivot point (M).  D is greater than C and A is less than C but both D and A are on the wrong side of C.  We can then swap the values that L and R point to.  In order to do a swap, we will need a temporary variable.  After doing the swap we can advance L and R respectively.
```
tmp = L (D)
L = R (A)
R = tmp
L = L + 1
R = R - 1
```

```
BACDE
  M
  L
  R
```

Since L is greater than or equal to R, we can assume we have reached the end and we have three pieces.
```
BA C DE
```

The only thing to do now is to rerun the algorithm on BA and DE.  

The algorithm is a bit more complex especially when there is a mid point selected.  For example,

```
ACBDE
```

B will end up being the midpoint.
```
ACBDE
L M R
```

L will advance to C, and R will advance to B because A is less than B and D and E are greater than B.
```
ACBDE
 LM
  R
```

In this case, L is still less than R, but R is equal to the midpoint.  If you swap C and B (L and R), the midpoint will also need to move.  The midpoint doesn't change value, it just changes location.  If R is equal to M during the swap, then M needs to point to L.  If L is equal to M during the swap, then M needs to point to R.

```
ABCDE
 MR
 L
```

Once L and R are advanced, they no longer will fit the rule of L being less than R, so the partition is done.
```
A B CDE
```

This illustrates that even choosing the median of 3 can cause an uneven split.  There is much more that can be done.  While the pure quicksort algorithm is relatively simple, our goal is to provide the "quickest" sort possible (or within reason).  Time to dive into some code.

The simplest quicksort algorithm choosing the last point as the pivot<br />
simple_quicksort.c:
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(char *a, char *b) {
  char tmp = *a;
  *a = *b;
  *b = tmp;
}

int find_pivot(char *low, char *high) {
  char *wp = low;
  char *p = low;
  while(p < high) {
    if(*p < *high) {
      swap(p, wp);
      wp++;
    }
    p++;
  }
  swap(wp, high);
  return wp-low;
}

void quicksort(char *low, char *high) {
  if(low < high) {
    int pivot = find_pivot(low, high);
    quicksort(low, low+pivot-1);
    quicksort(low+pivot+1, high);
  }
}

int main( int argc, char *argv[]) {
  for( int i=1; i<argc; i++ ) {
    char *s = strdup(argv[i]);
    quicksort(s, s+strlen(s)-1);
    printf("%s => %s\n", argv[i], s);
    free(s);
  }
  return 0;
}
```

Save the above code as simple_quicksort.c and run the following command to build simple_quicksort.  The -o option indicates the name of the program that you wish to build.  In this case, we are building a program named simple_quicksort.
```
gcc simple_quicksort.c -o simple_quicksort
```

Run the application
```
./simple_quicksort this is a simple version of the quicksort algorithm
```

and it should output
```
this => hist
is => is
a => a
simple => eilmps
version => einorsv
of => fo
the => eht
quicksort => cikoqrstu
algorithm => aghilmort
```

My hope is that the code is fairly clear in light of the earlier discussion of how quicksort works, but I will explain it anyways as this code will end up being very different by the time I'm done.

The C language has a number of built-in functions.  The functions are defined in various header files such as the three listed below.  In our program, printf is found in stdio.h, strlen and strdup are found in string.h, and free is found in stdlib.h.  Many examples will include the following three header files.
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
```

It is typically a good idea to begin exploring a C file from the main function.  The main function is the entry point to the program.  The main function returns an int (typically a 4 byte number which can range from -2,147,483,648 to 2,147,483,647).  A zero is typically returned from main functions to indicate that the program executed successfully.  The <b>int argc</b> is the first parameter and indicates how many command line arguments exist.  In the example above, there were 9 words or arguments.  argc would have a value of 10 because the command itself is considered the first argument.  <b>char *argv[]</b> is a list of the actual argument values.  It can be read as <i>an array of <b>char *</b></i>.  A <b>char *</b> simply means that the variable points to a specific character or byte in RAM (or memory).  For example, the last parameter is algorithm in the example above.  The last argument would point to the letter <i>a</i> in <i>algorithm</i>.  In C, the pointer refers to a specific byte in memory.  It technically also points to the string <i>algorithm</i>, but that is only a convention.  In C, strings are terminated with a zero byte (and not the printable number 0).  The string algorithm is nine letters long.  To represent it in memory, there is an extra byte at the end which is a non-printable zero character.  All functions in C will take a similar form.  <i>return type<i/> <b>function name</b>(<i>parameters</i>) {}.
```c
int main( int argc, char *argv[]) {
  ...
  return 0;
}
```

```c
for( int i=1; i<argc; i++ ) {
  char *s = strdup(argv[i]);
  quicksort(s, s+strlen(s)-1);
  printf("%s => %s\n", argv[i], s);
  free(s);
}
```

```c
void swap(char *a, char *b) {
  char tmp = *a;
  *a = *b;
  *b = tmp;
}
```


# Choosing the Pivot Point

## First, Middle, Last, or Random Points

All of these options will suffer from




One property of the sorted set is that in order to find the median, you must check that the first, middle, and last are in order.  An improvement to the algorithm in my opinion would be to possibly consider a couple more sample points when this condition occurs depending upon the number of elements and check if the data is originally sorted in the first place.  Since ACE are originally sorted, then perhaps the whole dataset is.  If the dataset is larger (>16 elements for example), then check 4 more points and if all remain in order, then check order of whole set.  My general intuition on this is that you should likely only run this as a test of the whole dataset to be sorted.  If the whole dataset has enough random points which are in order, then it is likely that the whole set is sorted.  At least in work I've done, I've found that data is frequently sorted when it enters the quicksort method.

```
ABCDEFGHIJKLMNOPQRSTUVWXYZ (since AMZ are in sorted order, look at D,I,Q, and V and check order of all 7 points)
^           ^            ^
   ^    ^       ^    ^
```

Since all 7 points are sorted, look for a case where an item and the one after it are out of order.  If one is found, resort to standard partitioning using M.  In addition, if at some point, points were out of order as in the following example (S and F are swapped).
```
ABCDESGHIJKLMNOPQRFTUVWXYZ (since AMZ are in sorted order, look at D,I,Q, and V and check order of all 7 points)
^           ^            ^
   ^    ^       ^    ^
```

The midpoint swapping can happen in reverse order from the point just before where the sort was violated.
```
ABCDESG
     ^
```

Then begin swapping the left side with the right side based upon the midpoint.
```
ABCDESGHIJKLMNOPQRFTUVWXYZ => ABCDEFGHIJKLMNOPQRSTUVWXYZ
     ^            ^
```

While this is sorted, the algorithm is not aware of that.  Continue until the split is made
