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

Tony Hoare chose the first item in the set to pivot upon.  Nico Lomuto chose to use the last item in the set to pivot upon.  Robert Sedgewick popularized the algorithm and suggested using either a random pivot or the median of the first, middle, and last item.  

Essentially, the key to solving quicksort efficiently is understanding worst case and trying to avoid them.  Much of the work revolves around choosing a pivot properly and deciding if it makes sense to choose an alternate algorithm once the subset becomes small enough.  If you always pivot on the last item of a sorted set, then you will have the worst case scenario.  If the set has N items, there there would be N splits.  Each split would require a scan of N items (decreasing by 1 for each successive split).  In addition to this, it is ideal for splits to produce three sets (left, pivot, and right).  The sorted set would continuously produce two sets (left and pivot).
