# The Red Black Tree
### [Table of Contents](README.md)

The red black tree is a mostly balanced binary search tree that was invented by Leonidas J. Guibas and Robert Sedgewick.  To understand the red black tree, it is good to have a thorough understanding of [binary search trees](10_binary_search_trees.md) first.  

## Why do we need balanced binary search trees?

To start, let's explore why balancing a binary search tree is important.

The following code is found in <i>illustrations/11_red_black_tree/1_binary_search_tree</i>
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
2. The root is black.
3. All leaves (NIL) are black.
4. If a node is red, then both its children are black.
5. Every path from a given node to any of its descendant NIL nodes contains the
  same number of black nodes.

My additional rules for clarification which are based upon the first 5 rules.
- If a node has one child, the child must be red
- If a node has two children, one or both of the children can be red if the parent is black
- If a node is red, it must have either two children which are black or no children at all.
- The parent of a red node must be black
- The black height of any leaf node must be the same (another way of stating 5)
- A red black tree often will have many more black nodes than red nodes.  This is okay and expected.  The red node is an indication that the tree may be somehow out of balance.  It is possible to have more red nodes than black nodes, but it isn't typical.
- A red black tree has a worst case of a 2logN depth, but is likely to maintain a logN depth or be very close to it.

## Building an intuition for how red black trees work

To begin, let's look at a few red black trees.  Markdown doesn't support printing color in code blocks, so the red nodes are not colored.  They are colored if you follow along in the terminal.  In order to accommodate, red nodes are suffixed with an r.  One of the rules for a red black tree is that all of the leaf nodes must have the same black height.  Instead of printing the depth of a node, I'm printing the black depth (or black height) of the nodes.  Black height can be calculated by simply counting the number of black nodes in the path to the root.


The following code is found in <i>illustrations/11_red_black_tree/2_red_black_tree</i>
```bash
cd $stla/illustrations/11_red_black_tree/2_red_black_tree
```

```bash
make
```

```bash
$ ./test_data_structure ABC
Creating red_black_tree for ABC
B1
| \
|  C1r
|     
A1r
```

The black height of A and C are 1 because there is one black node in their path to the root (B).

```bash
$ ./test_data_structure ABCDEFGH
Creating red_black_tree for ABCDEFGH
D1
| \
|  F1r
|  |  \
|  E2  G2
|        \
B1r       H2r
|  \         
A2  C2
```

- The black height of A is 2 because A and D are black.  
- The black height of H is 2 because G and D are black.  
- The black height of all of the leaf nodes are 2.  
- H is the only child of G and because of that H is red.
- Because H is red, G is black.
- F and B are red and have two black children.
- H is red and has no children.
- D is the root, so it is black.

```bash
$ ./test_data_structure GERMANY
Creating red_black_tree for GERMANY
G1
| \
E2 N1r
|  |  \
|  M2  R2
|        \
A2r       Y2r
```

- The black height of the leaf nodes is 2.
- N is red, so both of its children are black.
- Y and A are only children, so they are red.
- G is the root, so it is black.

If you think about removing nodes, you can remove A and Y, making E and R leaf nodes, without affecting black height equality.
```bash
G1
| \
E2 N1r
   |  \
   M2  R2
```

If you remove M
```bash
G1
| \
E2 N1r
      \
      R2
```

- E and R have a black height of 2, so the height is okay
- R is an only child, but is black (wrong)
- N is red and only has one black child (wrong)

If we could swap the colors of N and R, we would have a valid red black tree as the only child R would be red.
```bash
G1
| \
E2 N2
     \
      R2r
```

We could also rotate R to the left through N and get

```bash
G1
| \
E2 R2
   |
   N2r
```

Typically when a node is rotated, the color remains the same of the node that is being rotated through (which ends up also being a color swap).  

## Rotations

The red black tree is balanced through rotations and changing colors.  The following examples will not include colors and are just made up.  A right rotation around R

```bash
G
|\
E R
  |
  N
```

would result in
```bash
G
|\
E R
   \
    N
```

A left rotation around G would result in
```bash
R
|\
G N
|
E
```

A right rotation around G would result in
```bash
R
|\
E N
 \
  G
```

In all cases after a rotation, the tree is still a valid binary search tree.

The implementation of the right rotation looks like the following.

```c
void rotate_right(node_t *A, node_t **root) {
  node_t *new_root = A->left;
  _swap_nodes(new_root, A, root);

  node_t *tmp = new_root->right;
  new_root->right = A;
  A->parent = new_root;

  A->left = tmp;
  if(A->left)
    A->left->parent = A;
}
```

When rotating to the right, the left node will become the parent and the current parent will move to the right of the parent.  

If the parent's parent is the root node, the parent's parent must relink to the left node.  Otherwise, the parent is the root node, the left node will become the new root node.  This is done through _swap_nodes.

```c
static inline void _swap_nodes(node_t *dest,
                               node_t *src,
                               node_t **root) {
  node_t *parent = src->parent;
  if(parent) {
    if(parent->left == src)
      parent->left = dest;
    else
      parent->right = dest;
    dest->parent = parent;
  }
  else {
    dest->parent = NULL;
    *root = dest;
  }
}
```

The old left node (which has become the parent) assigns its right pointer to the old parent node.  The parent of the old parent is set to the old left node.
```c
node_t *tmp = new_root->right;
new_root->right = A;
A->parent = new_root;
```

If the left node had a right child, that would have been saved in tmp.  Assign this to the left of the old parent.  If tmp was not NULL, set its parent to the old parent.
```c
A->left = tmp;
if(tmp)
  tmp->parent = A;
```

Rotating to the left is the same as rotating to the right, except you swap the words left and right in the explanation and code above.  For completeness, the two functions are included below.

```c
void rotate_right(node_t *A, node_t **root) {
  node_t *new_root = A->left;
  _swap_nodes(new_root, A, root);

  node_t *tmp = new_root->right;
  new_root->right = A;
  A->parent = new_root;

  A->left = tmp;
  if(tmp)
    tmp->parent = A;
}
```

```c
void rotate_left(node_t *A, node_t **root) {
  node_t *new_root = A->right;
  _swap_nodes(new_root, A, root);

  node_t *tmp = new_root->left;
  new_root->left = A;
  A->parent = new_root;

  A->right = tmp;
  if(tmp)
    tmp->parent = A;
}
```

The differences are...
```c
void rotate_right(node_t *A, node_t **root) {
  node_t *new_root = A->left;
```

to
```c
void rotate_left(node_t *A, node_t **root) {
  node_t *new_root = A->right;
```

These lines change from
```c
new_root->right = A;
A->parent = new_root;
```

to
```c
node_t *tmp = new_root->left;
new_root->left = A;
```

The last change is
```c
A->left = tmp;
```

to
```c
A->right = tmp;
```

Notice that the change is literally to swap the words left and right.

During most of the rotations, you will also need to swap colors.
```c
void rotate_right_and_swap_colors(node_t *A, node_t **root) {
  size_t tmp = A->color;
  A->color = A->left->color;
  A->left->color = tmp;
  rotate_right(A, root);
}

void rotate_left_and_swap_colors(node_t *A, node_t **root) {
  size_t tmp = A->color;
  A->color = A->right->color;
  A->right->color = tmp;
  rotate_left(A, root);
}
```

These functions swap colors and then do the rotation.
