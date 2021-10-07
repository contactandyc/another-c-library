---
path: "/11-balancing-binary-search-trees"
posttype: "tutorial"
title: "11. Balancing Binary Search Trees"
---

This builds significantly on the last section about [binary search trees](../10-binary-search-trees/index.md).  To start, let's explore why balancing a binary search tree is important.

The following code is found in <i>illustrations/11\_balancing\_binary\_search\_trees/1\_binary\_search\_tree</i>
```
cd $ac/illustrations/11_balancing_binary_search_trees/1_binary_search_tree
```

```
make
```

In this case, I've not included examples in the Makefile.  I've also commented out printing the data structure using iteration and reverse iteration as it isn't relevant to understanding how the red-black tree works.

## Why balancing is important

To understand why balancing a binary search tree is important, let's look at some worst-case and bad scenarios.

One worst case scenario of a binary search tree
```
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

Another worst-case scenario
```
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
```
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

Yet another worst-case scenario.
```
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

The leaf nodes are the nodes that don't have children.  In the example above, all of the leaves have a depth of 3, except l, which has a depth of 4.  At the first level of a binary search tree, a fully balanced tree can have at most 1 node (the root node).  At the second level, 2 nodes (+1 for the first level).  At the third level, 4 nodes (+3 for the first and second), and so on.  The example above has 8 nodes, and the entire first, second, and third levels are full.  This tree is balanced.

A balanced binary search tree is useful in that you are guaranteed that finding a node will take O(logN) time where N is the number of elements in the tree.  A binary search tree that isn't balanced has a worst case of O(N) time.  This isn't a big deal if you have 5-10 nodes, but if you have 1 million nodes, a balanced binary search tree will take at most 21 operations.  If the tree is not balanced, it might take 1 million operations!  You will often see logN used in describing how long an algorithm takes.  The base of log, in this case, is 2 (since computers think in terms of 0s and 1s or powers of 2).


The red-black tree is a mostly balanced binary search tree that was invented by Leonidas J. Guibas and Robert Sedgewick.  Certain properties make a red-black tree valid.  

## Properties of a red-black tree

Red–black tree Properties (https://en.wikipedia.org/wiki/Red–black\_tree)

1. Each node is either red or black.
2. The root is black.
3. All leaves (NIL) are black.
4. If a node is red, then both its children are black.
5. Every path from a given node to any of its descendant NIL nodes contains the same number of black nodes.

My additional rules for clarification which are based upon the first 5 rules.
- If a node has one child, the child must be red (and the parent of that child must be black)
- If a parent is red, the children must be black.  If the parent is black, the children can be red or black.
- If a node has two children, one or both of the children can be red if the parent is black
- If a node is red, it must have either two children who are black or no children at all.
- The parent of a red node must be black
- The black height of any leaf node must be the same (another way of stating 5)
- A red-black tree often will have many more black nodes than red nodes.  This is okay and expected.  The red node is an indication that the tree may be somehow out of balance.  It is possible to have more red nodes than black nodes, but it isn't typical.
- A red-black tree has a worst case of a 2logN depth but is likely to maintain a logN depth or be very close to it.  The red-black tree would only reach 2logN if every other node from the root (which must be black) is red.  Given that this is very unlikely, a red-black tree tends to be closer to logN depth.


The red-black tree uses coloring and tree rotations to balance the binary search tree.  

## Coloring

In a red-black tree, the red and black colors are introduced.  Typically, this is represented as a single bit with 0 meaning black and 1, meaning red.  

To color a node red, set the color to RED (RED is defined as 1).
```c
void color_node_red( node_t *node ) {
  node->color = RED;
}
```

Likewise, to color a node black, set the color to BLACK (BLACK is defined as 0).
```c
void color_node_black( node_t *node ) {
  node->color = BLACK;
}
```

Recoloring a node involves painting a node RED and its children (which both must exist) BLACK.
```c
void recolor( node_t *n ) {
  n->color = RED;
  n->left->color = n->right->color = BLACK;
}
```

## Rotations

Rotations are used to balance binary search trees when using the red-black tree algorithm.  The following examples will not include colors and are just made up.  A right rotation around R

```
G
|\
E R
  |
  N
```

would result in
```
G
|\
E N
   \
    R
```

A left rotation around G would result in
```
N
|\
G R
|
E
```

A right rotation around G would result in
```
N
|\
E R
 \
  G
```

In all cases, after a rotation, the tree is still a valid binary search tree.  To do a right rotation around a node, that node must reference a left node.  The left node and itself will exchange colors.  When doing a left rotation around a node, that node must reference a right node.  The right node and itself will exchange colors.

rotate\_left does the following.
- assign new\_root to A->right.
- swap the color between A and new\_root.
- if A has a parent, link the parent to new\_root; otherwise, link root to new\_root.
- put A to the left of new\_root and what was previously to the right of new\_root to the left of A.

```c
void rotate_left(node_t *A, node_t **root) {
  node_t *new_root = A->right;
  size_t tmp_color = A->color;
  A->color = new_root->color;
  new_root->color = tmp_color;
  node_t *parent = A->parent;
  if(parent) {
    if(parent->left == A)
      parent->left = new_root;
    else
      parent->right = new_root;
    new_root->parent = parent;
  }
  else {
    new_root->parent = NULL;
    *root = new_root;
  }

  node_t *tmp = new_root->left;
  new_root->left = A;
  A->parent = new_root;

  A->right = tmp;
  if(tmp)
    tmp->parent = A;
}
```

Rotating to the right is similar to rotating to the left (except you need to swap left and right).

I've written a tool to help you to understand how colors and rotations work.

The following code is found in <i>illustrations/11\_balancing\_binary\_search\_trees/2\_tree\_operations</i>
```
cd $ac/illustrations/11_balancing_binary_search_trees/2_tree_operations
```

```
make
```

```
./tree_operations
```

You will be prompted with the following...
```
(i)nsert, (e)rase, (r)ight_rotate, (l)eft_rotate, (R)ed, (b)lack, re(c)olor, (h)elp, (q)uit
```

typing
```
i a
```

outputs
```
(a0)

The root is not black!

(i)nsert, (e)rase, (r)ight_rotate, (l)eft_rotate, (R)ed, (b)lack, re(c)olor, (h)elp, (q)uit
```

If the node is in parenthesis, it is red.  On your terminal, it should print in red color as well.  Whenever you insert a node, it is painted red.  You can use the tool to try and turn this into a proper red-black tree.  In the red-black tree, the root is always black.  We can paint a black by typing the following.

typing
```
b a
```

outputs
```
a1

Starting with a valid red black tree
====================================

Operation: insert a
(a0)

The root is not black!

Operation: black a
a1

The above tree is a valid red-black tree
```

The first operation **insert a** broke the red-black tree rules.  It was fixed by painting the root node black.

typing
```
i b
```

outputs
```
a1
  \
   (b1)

Starting with a valid red black tree
====================================
a1


Operation: insert b
a1
  \
   (b1)


The above tree is a valid red-black tree
```

In this case, nothing needed to be done because b's parent was black.

typing
```
i c
```

outputs
```
a1
  \
   (b1)
       \
        (c1)

(b1) has a red right child and is red
(c1) has a red parent and is red
```

In this case, c's parent b is red.  If we rotate around a to the left, it should fix it.

typing
```
l a
```

outputs
```
b1
| \
|  (c1)
|      
(a1)

Starting with a valid red black tree
====================================
a1
  \
   (b1)


Operation: insert c
a1
  \
   (b1)
       \
        (c1)

(b1) has a red right child and is red
(c1) has a red parent and is red

Operation: left_rotate a
b1
| \
|  (c1)
|      
(a1)

The above tree is a valid red-black tree
```

typing
```
i d
```

outputs
```
b1
| \
|  (c1)
|      \
(a1)    (d1)

(c1) has a red right child and is red
(d1) has a red parent and is red
```

Since the parent (c) and the uncle (a) are both red, we can try recoloring around the grandparent (b).

typing
```
c b
```

outputs
```
(b0)
|   \
a1   c1
       \
        (d1)

The root is not black!
```

If the tree is balanced, we can always just paint the root node black.

typing
```
b b
```

outputs
```
b1
| \
a2 c2
     \
      (d2)

Starting with a valid red black tree
====================================
b1
| \
|  (c1)
|      
(a1)

Operation: insert d
b1
| \
|  (c1)
|      \
(a1)    (d1)

(c1) has a red right child and is red
(d1) has a red parent and is red

Operation: color b
(b0)
|   \
a1   c1
       \
        (d1)

The root is not black!

Operation: black b
b1
| \
a2 c2
     \
      (d2)      

The above tree is a valid red-black tree
```

typing
```
i e
```

outputs
```
b1
| \
a2 c2
     \
      (d2)
          \
           (e2)

(d2) has a red right child and is red
(e2) has a red parent and is red
```

This can be resolved through a left rotation.

typing
```
l c
```

outputs
```
b1
| \
a2 d2
   | \
   |  (e2)
   |      
   (c2)

Starting with a valid red black tree
====================================
b1
| \
a2 c2
     \
      (d2)


Operation: insert e
b1
| \
a2 c2
     \
      (d2)
          \
           (e2)

(d2) has a red right child and is red
(e2) has a red parent and is red

Operation: left_rotate c
b1
| \
a2 d2
   | \
   |  (e2)
   |      
   (c2)


The above tree is a valid red-black tree
```

The tool will also allow you to erase nodes.  Let's erase a to start.

typing
```
e a
```

outputs
```
b1
  \
   d2
   | \
   |  (e2)
   |      
   (c2)

b1 has one right child, and it isn't red
```

We can try a left rotation around b

typing
```
l b
```

outputs
```
d1
| \
|  (e1)
|      
b2
  \
   (c2)

(e1) has a different black height than (c2)
```
If we color e black, the tree will be a proper red-black tree.

typing
```
b e
```

outputs
```
d1
| \
|  e2
|    
b2
  \
   (c2)

Starting with a valid red black tree
====================================
b1
| \
a2 d2
   | \
   |  (e2)
   |      
   (c2)

Operation: erase a
b1
  \
   d2
   | \
   |  (e2)
   |      
   (c2)

b1 has one right child, and it isn't red

Operation: left_rotate b
d1
| \
|  (e1)
|      
b2
  \
   (c2)

(e1) has a different black height than (c2)

Operation: black e
d1
| \
|  e2
|    
b2
  \
   (c2)

The above tree is a valid red-black tree
```

I'll reference this tool in the explanation of the red-black tree that comes next.  Hopefully, the red-black tree will make more sense as we've already seen how much of it works.

The tool (tree\_operations) can also start with a valid red-black tree, which you can modify by specifying a sequence of characters in the order in which you wish the characters to be inserted.

```
$ ./tree_operations ABC
B1
| \
|  (C1)
|      
(A1)
```

```
$ ./tree_operations ABCD
B1
| \
A2 C2
     \
      (D2)
```

```
$ ./tree_operations BADC
B1
| \
A2 D2
   |
   (C2)
```

```
$ ./tree_operations ABCDEFGHIJKLMNOPQRSTUVWXYZ
H1
| \
|  P2
|  | \
|  |  (T2)
|  |  |   \
|  |  R3   V3
|  |  | \  | \
|  |  Q4 | U4 (X3)
|  |     |    |   \
|  |     S4   W4   Y4
|  |                 \
|  (L2)               (Z4)
|  |   \                  
|  J3   N3
|  | \  | \
|  I4 | M4 O4
|     |      
D2    K4
| \     
|  F3
|  | \
|  E4 G4
|       
B3
| \
A4 C4
```

Once the tool is loaded, you can insert or erase nodes and try and figure out how to make it work as a red-black tree again.

[Table of Contents (only if viewing on Github)](../../../README.md)
