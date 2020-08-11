---
title: Improving Quicksort
description: A new, faster sorting algorithm  
---

Quicksort is an extremely popular sorting algorithm developed by Tony Hoare. His basic intuition was that sorting a set of items could be done using a sequence of swaps around a pivot point. The pivot point would be used to divide the data such that all data less than or equal to the pivot would sit on one side and the rest of data would sit on the other side.  I've noticed an improvement that doesn't seem to be found in any of the implementations.  The idea is explored at the end (feel free to skip the basic description of quicksort if you know it).

> AFDBKDJE  

For example, if the last item (E) was was chosen as the pivot amongst the set above. ADBD would be to the left of the pivot and FKJ would be to the right of the pivot.

> ADBD E FKJ

We could then repeat the process of choosing the last item within each set (choose the smaller set first using J as the pivot and place F to the left and K to the right)

> ABDB E F J K

Once a set has one item, the set is in its proper place. The only remaining part is ABDB and we will again choose the last item as the pivot.

> A B DB E F J K

Finally, DB will be sorted by choosing B and then placing D to the right.

> A B B D E F J K

Worst case scenarios and choosing a pivot

In the above example, the last item was chosen as the pivot. If the set was already sorted, it would yield the worst case scenario. Consider the following.

> ABCDE

choose E as midpoint (ABCD compared against E)

> ABCD E

choose D as midpoint (ABC compared against D)

> ABC D E

choose C as midpoint (AB compared against C)

> AB C D E

choose B as midpoint (A compared against B)

> A B C D E

In this case, the sort would take 10 comparisons. Choosing the first point would yield a similar result. If the midpoint was chosen, then the sort worst of data being sorted would have been avoided.

> ABCDE

choose C as midpoint (ABDE compared)

> AB C DE

choose A as midpoint (mid of 2 choosing first, B compared)

> A B C DE

choose D as midpoint (E compared)

> A B C D E

By choosing the midpoint, it required one less pivot selection and the number of comparisons is limited to 6.  This still has a worst case where all items are equal.

> AAAAA

choose midpoint A (compare AAAA)

> A AAAA

choose midpoint A (compare AAA)

> A A AAA

choose midpoint A (compare AA)

> A A A AA

choose midpoint A (compare A)

> A A A A A

The worst case scenario for the partitioning is when a single item is split off.  The best case scenario is when the partitioning equally splits the remaining items.  All items being equal is a common case.  It can be guarded against if it is noted that the pivot created two sets (one where all items were greater or equal and the other being the pivot itself).  In this case, compare the pivot to the next item and while they are equal, make the pivot set greater.  Since all of the items are equal, the pivot set doesn't need to be further sorted.

> AAAAA

choose midpoint A (compare AAAA)

> A AAAA

Notice that A is by itself and first.  Compare A to the next items until items are not equal (four more comparisons are done).

> AAAAA

Choosing a midpoint of N items.

Robert Sedgewick proposed choosing a midpoint of 3 to 9 items.  The following example will choose the midpoint of 5 items from the original example.

> AFDBKDJE  

The 5 items chosen are the first, the last, and 3 evenly spaced items in between (AFBDE).  To find the midpoint of 5 points, min-heapify the last 3 points and then only add the other two points if they are greater than the mid point.  If they are greater, swap the min value.

> BDE

Then compare A against B, since A is less than B, do nothing.  

Compare F against B, since it is greater than B, swap B and push F onto the min-heap.

> A B FDE

Becomes

> A B DEF

The midpoint D is then used to do the split.  In the process of sorting the 5 elements, we actually have sorted some of the items in the original set.

> ABDDKEJF  

We also know that AB and EF are to the left and the right of the pivot D, so we can move them into the first two places and the last two places.  In this case, B is already in the second place.  E and J will get swapped.

> ABDDKJEF

The normal partitioning can continue with the middle DDKJ items as the AB and EF are already known.  We've chosen D as the pivot, so the end result is

> AB D DKJEF

A couple of things to notice here.  Even though we chose the midpoint of 5, we didn't end up with great partitioning.  If the first, last, or mid was chosen it would have only been slightly worse.  In addition to this, we had to sort sparse items to find the median of 5.  If the 5 items chosen were all equal (and perhaps D), then there would be 2 Ds to the left of the pivot and 2 Ds to the right.  This is okay as the eventual sort will move the 2 Ds to the left to be next to the pivot (along with any other Ds).  The probability only slightly increases no matter which method is used.  The median of 3 or 5 does yield one slight advantage in that 2 or 3 items respectively will be partitioned off at each stage.

### An important intuition to quicksort

Quicksort performs best when data is randomized.  Having partially sorted data mixed into quicksort doesn't generally improve the overall performance of the algorithm.  Quicksort performs well when midpoints are chosen and it subsequently will often somewhat randomize perhaps previously partially sorted blocks.

## The Improvement to Quicksort

The improvement that I've found is to consider Robert Sedgewick's choosing of the midpoint of N points.  During this process, one can know by the lack of reshuffling that the set of N points is in order (or in reverse order).  If it is known that the set of N points is in order and N is sufficiently large (perhaps 5-9 points), then you can check the original set to see if the set itself was in order.  The larger N is, the less likely that the set would randomly be in order.  If one were to consider the likelihood of 5 randomly or spatially selected points being in order and the data not being in order, one would only check the order less than 2% of the time.  If the number of points is increased to 9, then the order would only need to be checked less than 0.01% of the time.   This should only be applied on the first iteration of partitioning as it is highly likely that the input to the sort is already sorted, all equal, or reversed.  It is very unlikely that a sub-partition is both sorted and in the right place.  Consider the following example.

> ABCDEFGHIJKLMNOQPRSTUVWXYZ

If 5 points are initially chosen AGNTZ.  During the heapify and selection of the midpoint, nothing would be changed.  Given that, compare the sequence in order to determine that the data is actually sorted.  If the set didn't have P and Q swapped and the items were in order, the order test would have been all that is needed to see that the data is sorted.  If the whole set was equal, this optimization would have also worked.

It is possible that the data isn't sorted (even if the 5 points are sorted) as in the example above where Q and P are swapped.  I'd suggest to make P the pivot and continue with the process as it is known that every point before the first case of data being out of order is less than the pivot.  If the data was out of order early (before the midpoint), then compare the midpoint and item that was out of order.  If the midpoint is less than the out of order element, swap the out of order element with the midpoint and continue with the process.

The second optimization is to consider the first and the last point.  If the first and the last point are reversed, consider reversing the comparisons to check that all 5 of the items are exactly reversed.  If they are reversed, then compare the set in exactly the same manner, except compare from the right backwards.  If all items are in reverse order do the standard reverse swapping method (swapping first and last towards the middle).  

If items are not sorted, the simplest option is to order the 5 points in the correct order, choose the midpoint, and continue with the normal sort.  Obviously, 5 points is somewhat arbitrary.  The more points that are initially chosen, the less likely the data will be falsely assumed to be in order.  However, even the cost of the check is largely mitigated in that a new pivot can be chosen.  A more complex solution might consider where the out of order element was found and continue with that pivot.

I've been writing an open source book (which is very much in its infancy) to explore algorithms and C found at https://github.com/contactandyc/another-c-library  Part of the demo, illustrates the timing as it relates to this improvement.  In general, this algorithm only takes steps forward with very little extra cost.  The timings from the demo are shown below.

My sort is called ac_sort (another c sort).  It uses a combination of algorithms internally (particularly for sets of items which are 5 or less, the sort is hard-coded).  The core of the sorting algorithm is based upon works by Roger Sedgewick with the improvement mentioned above.

ac_sort
* random - 312,257 nanoseconds
* already sorted - 17,007 nanoseconds
* reversed - 17,722 nanoseconds

The regular qsort
* random - 315,232 nanoseconds
* already sorted - 201,070 nanoseconds
* reversed - 199,330 nanoseconds

The demo directory has a tool called quicksort_demo which shows the following results when sorting 64 bit integers.

```
./quicksort_demo A 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
Manual test...
system qsort: 120.657ns
ac_sort: 16.636ns

./quicksort_demo A 1000
Ordered test...
system qsort: 4055.900ns
ac_sort: 561.300ns
Equal test...
system qsort: 2545.000ns
ac_sort: 488.800ns
Reverse test...
system qsort: 21440.100ns
ac_sort: 719.900ns
Slightly out of ordered test1...
system qsort: 3945.300ns
ac_sort: 4836.600ns
Slightly out of ordered test2...
system qsort: 4023.900ns
ac_sort: 4843.000ns
Random test...
system qsort: 55226.700ns
ac_sort: 9078.500ns
```

I've run many different tests and compared it with various implementations of quicksort (including the C++ sort) and found that the improvement is better across the board.  In some cases, the C++ sort is more efficient than ac_sort and vice-versa for unsorted data.  I hope that this improvement can be adopted by the different implementations of quicksort!  Please share if you like this improvement to an almost 80 year old algorithm.

I'm working on a detailed explanation of how the ac_sort algorithm works and will publish it soon!
