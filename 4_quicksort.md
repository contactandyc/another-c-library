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

Median of 3 for BDEAC becomes same as choosing last element in this case.
```
BDEAC (median of B, E, and C is C)
    ^
```

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
