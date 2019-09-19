# The Red Black Tree

The red black tree is a mostly balanced binary search tree that was invented by Leonidas J. Guibas and Robert Sedgewick.  To understand the red black tree, it is good to have a thorough understanding of [binary search trees](10_binary_search_trees.md) first.  

## Why do we need balanced binary search trees?

To start, let's explore why balancing a binary search tree is important.

The following code is found in <i>illustrations/2_timing/1_timer</i>
```bash
cd $stla/illustrations/11_red_black_tree/1_binary_search_tree
```

```bash
make
```

In this case, I've not included examples in the Makefile.  I've also commented out printing the data structure using iteration and reverse iteration as it isn't relevant to understanding how the red black tree works.

To understand why balancing a binary search tree is important, let's look at some worst case and bad scenarios.

One worst case scenario
```bash
$ ./test_data_structure ABCDEFGHI
Creating binary_search_tree for ABCDEFGHI
A1
  \
   B2
     \
      C3
        \
         D4
           \
            E5
              \
               F6
                 \
                  G7
                    \
                     H8
                       \
                        I9
```

Another worst case scenario
```bash
$ ./test_data_structure IHGFEDCBA
Creating binary_search_tree for IHGFEDCBA
I1
|
H2
|
G3
|
F4
|
E5
|
D6
|
C7
|
B8
|
A9
```


A mixed case where the tree isn't balanced
```bash
$ ./test_data_structure IHGFEDCBAabcdef
Creating binary_search_tree for IHGFEDCBAabcdef
I1
| \
H2 a2
|    \
G3    b3
|       \
F4       c4
|          \
E5          d5
|             \
D6             e6
|                \
C7                f7
|                   
B8
|
A9
```

Yet another worst case scenario.
```bash
$ ./test_data_structure Izpagkml
Creating binary_search_tree for Izpagkml
I1
  \
   z2
   |
   p3
   |
   a4
     \
      g5
        \
         k6
           \
            m7
            |
            l8
```

The example above could be said to be balanced if it looked like the following.

```
k1
| \
|  p2
|  | \
|  m3 z3
|  |    
|  l4
|    
a2
| \
I3 g3
```

The leaf nodes are the nodes which don't have children.  In the example above, all of the leaves have a depth of 3, except l which has a depth of 4.  At the first level of a binary search tree, a fully balanced tree can have at most 1 node (the root node).  At the second level, 2 nodes (+1 for the first level).  At the third level, 4 nodes (+3 for the first and second), and so on.  The example above has 8 nodes and the entire first, second, and third levels are full.  This tree is balanced.

A balanced binary search tree is useful in that you are guaranteed that finding a node will take O(logN) time where N is the number of elements in the tree.  A binary search tree that isn't balanced has a worst case of O(N) time.  This isn't a big deal if you have 5-10 nodes, but if you have 1 million nodes, a balanced binary search tree will take at most 21 operations.  If the tree is not balanced, it might take 1 million operations!  You will often see logN used in describing how long an algorithm takes.  The base of log in this case is 2 (since computers think in terms of 0s and 1s or powers of 2).

## The properties of a red black tree

Red–black tree Properties (https://en.wikipedia.org/wiki/Red–black_tree)

1. Each node is either red or black.
2. The root is black. This rule is sometimes omitted. Since the root can always
  be changed from red to black, but not necessarily vice versa, this rule has
  little effect on analysis.
3. All leaves (NIL) are black.
4. If a node is red, then both its children are black.
5. Every path from a given node to any of its descendant NIL nodes contains the
  same number of black nodes.

My additional rules for clarification which are based upon the first 5 rules.
- If a node has one child, the child must be red
- If a node has two children, the children may be black or red
- The parent of a red node must be black
- The black height of any leaf node must be the same (another way of stating 5)
- A red black tree often will have many more black nodes than red nodes.  This is okay and expected.  The red node is an indication that the tree may be somehow out of balance.  It is possible to have more red nodes than black nodes, but it isn't typical.
- A red black tree has a worst case of a 2logN depth, but is likely to maintain a logN depth or be very close to it.

## Building an intuition for how red black trees work

To begin, let's look at a few red black trees.  If you can follow
```html
<div style="background-color: blue">
$ ./test_data_structure ABC
Creating red_black_tree for ABC
B1
| \
A1 C1
</div>
```
