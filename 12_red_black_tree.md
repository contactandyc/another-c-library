[Table of Contents](README.md)  - Copyright 2019 Andy Curtis

# The Red Black Tree

The red black tree is a mostly balanced binary search tree that was invented by Leonidas J. Guibas and Robert Sedgewick.
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

The red black tree is balanced through rotations and changing colors which were discussed in [Balancing Binary Search Trees](11_balancing_binary_search_trees.md).  Make sure the tree_operations tool is built from the last section if you want to follow along.

```bash
cd $stla/illustrations/11_balancing_binary_search_trees/2_tree_operations
make
```

Then run
```bash
$ $stla/bin/tree_operations ABC
B1
| \
|  (C1)
|      
(A1)

(i)nsert, (e)rase, (r)ight_rotate, (l)eft_rotate, (R)ed, (b)lack, re(c)olor, (h)elp, (q)uit
q
```

To make sure that it is working.


## Insert

The red black tree functions in many ways similar to the binary search tree.  Iterating and finding are exactly the same.  Erasing and inserting are done in virtually the same way, except once an item is inserted or erased, the color must be fixed.  The difference between the binary search tree and red black tree node_insert is shown below.

binary_search_tree.c
```c
bool node_insert(node_t *node_to_insert, node_t **root) {
  node_t **n = root, *parent = NULL;
  while (*n) {
    parent = *n;
    if (node_to_insert->key < parent->key)
      n = &(parent->left);
    else if (node_to_insert->key > parent->key)
      n = &(parent->right);
    else
      return false;
  }

  node_to_insert->parent = parent;
  node_to_insert->left = node_to_insert->right = NULL;
  *n = node_to_insert;
  return true;
}
```

red_black_tree.c
```c
void red_black_insert(node_t *node, node_t **root);

bool node_insert(node_t *node_to_insert, node_t **root) {
  node_t **n = root, *parent = NULL;
  while (*n) {
    parent = *n;
    if (node_to_insert->key < parent->key)
      n = &(parent->left);
    else if (node_to_insert->key > parent->key)
      n = &(parent->right);
    else
      return false;
  }

  node_to_insert->parent = parent;
  node_to_insert->left = node_to_insert->right = NULL;
  *n = node_to_insert;
  red_black_insert(node_to_insert, root);
  return true;
}
```

The difference is..

red_black_tree.c
```c
void red_black_insert(node_t *node, node_t **root);
...
red_black_insert(node_to_insert, root);
```

The red_black_insert method in red_black_tree.c
```c
void red_black_insert(node_t *node, node_t **root) {
  node->color = RED;
  node_t *parent, *grandparent, *uncle;

  while (true) {
    parent = node->parent;
    if(!parent) {
      node->parent = NULL;
      node->color = BLACK;
      break;
    }

    if(parent->color == BLACK)
      break;

    grandparent = parent->parent;
    if(grandparent->left == parent) {
      uncle = grandparent->right;
      if(uncle && uncle->color == RED) {
        // recolor grandparent
        grandparent->color = RED;
        parent->color = uncle->color = BLACK;
        node = grandparent;
        continue;
      }

      if(parent->right == node)
        rotate_left(parent, NULL);
      rotate_right_and_swap_colors(grandparent, root);
      break;
    }
    else {
      uncle = grandparent->left;
      if(uncle && uncle->color == RED) {
        // recolor grandparent
        grandparent->color = RED;
        parent->color = uncle->color = BLACK;
        node = grandparent;
        continue;
      }

      if(parent->left == node)
        rotate_right(parent, NULL);
      rotate_left_and_swap_colors(grandparent, root);
      break;
    }
  }
}
```

It is assumed that node is linked into its proper parent and that the node is a leaf node with left and right pointers set to NULL.  The red black tree always initially paints the given node red.
```c
void red_black_insert(node_t *node, node_t **root) {
  node->color = RED;
```

The insert operation will need to look at the parent, the grandparent, and the uncle (the sibling of the parent).  Declare these variables for later use.
```c
  node_t *parent, *grandparent, *uncle;
```

The red black tree insert may need to recurse.  Many recursion problems (this one included) can be written as a loop.  The loop will continue forever until a break is called.  
```c
while (true) {
  parent = node->parent;
```
At this point in the code, the node is always red.

The first check is to see if the given node is the root node.  If the node doesn't have a parent, it is a root node.  Root nodes are colored black and then we are done (break out of the while loop).    
```c
  if(!parent) {
    node->parent = NULL;
    node->color = BLACK;
    break;
  }
```

If the parent is black we are done as having a red leaf following a black parent is always valid.
```c
  if(parent->color == BLACK)
    break;
```

The parent is valid and it must be red (as it must be red or black and it was determined to not be black in the last block of code).  It is a violation of the red black tree to have two red nodes in a row.  Get the grandparent (the parent's parent).
```c
  grandparent = parent->parent;
```

The red black tree insert operation consider's the node's uncle's color.  The uncle would be the grandparent's other child.  If the grandparent->left == parent, then the uncle is the right node.  Otherwise, the uncle is the left node.  The else block is a mirror of the if block, switching every instance of left with right.
```c
  if(grandparent->left == parent) {
    uncle = grandparent->right;
    ...
  }
  else {
    uncle = grandparent->left;
    ...
  }
```

The next case to test is if the uncle exists and the uncle's color is red.  At this point the parent and the uncle are both red.  The red black tree needs to maintain a constant black height.

```
$ $stla/bin/tree_operations FDGBE
F1
| \
|  G2
|    
D2
| \
|  (E2)
|      
(B2)

(i)nsert, (e)rase, (r)ight_rotate, (l)eft_rotate, (R)ed, (b)lack, re(c)olor, (h)elp, (q)uit
i A
F1
| \
|  G2
|    
D2
| \
|  (E2)
|      
(B2)
|   
(A2)

(A2) has a red parent and is red
(B2) has a red left child and is red

(i)nsert, (e)rase, (r)ight_rotate, (l)eft_rotate, (R)ed, (b)lack, re(c)olor, (h)elp, (q)uit
```

Prior to inserting node (A2), notice that the leaf nodes (B2), (E2), and G2 all have a black height of 2 meaning that there are only 2 black nodes in the path from the root to each of the leaf nodes.  If you recolor (B2) and (E2) black and change D2 to be red, it doesn't change the black height of any of the leaf nodes.

In the tool you can type
```
c D
```
to recolor and get the following partial output
```
F1
| \
|  G2
|    
(D1)
|   \
B2   E2
|      
(A2)
```

Notice that in this case, the recoloring created a valid red black tree.  The black height is 2 to every leaf node.  The root is black.  There are not two red nodes in a row.  In the one case (A2) where a node only has a single child, the child is red.

To recap, if the parent and uncle are red, paint the parent and uncle black and the grandparent red.  It's possible that the grandparent's parent was also red.  To handle this case, we can repeat all of the tests recursively (since the recursion is simple, continuing in a while loop works by changing the node to the grandparent - which was painted red to maintain the rule that the loop always starts with a red node).

The following example adds the letter 0 (zero) to the left of (A2).  This creates the same case as above, but when recoloring happens (c B), (B1) and (D1) are both red. D becomes the new node. It is an example where recoloring creates a parent and child which are both red.

This is illustrated by the tool below.
```bash
$ $stla/bin/tree_operations FDGBEABCDEFGHI
F1
| \
|  H2
|  | \
|  |  (I2)
|  |      
|  (G2)
|      
(D1)
|   \
B2   E2
| \    
|  (C2)
|      
(A2)

(i)nsert, (e)rase, (r)ight_rotate, (l)eft_rotate, (R)ed, (b)lack, re(c)olor, (h)elp, (q)uit
i 0
F1
| \
|  H2
|  | \
|  |  (I2)
|  |      
|  (G2)
|      
(D1)
|   \
B2   E2
| \    
|  (C2)
|      
(A2)
|   
(02)

(02) has a red parent and is red
(A2) has a red left child and is red

(i)nsert, (e)rase, (r)ight_rotate, (l)eft_rotate, (R)ed, (b)lack, re(c)olor, (h)elp, (q)uit
c B
F1
| \
|  H2
|  | \
|  |  (I2)
|  |      
|  (G2)
|      
(D1)
|   \
|    E2
|      
(B1)
|   \
A2   C2
|      
(02)

(B1) has a red parent and is red
(D1) has a red left child and is red
```

```c
  if(grandparent->left == parent) {
    uncle = grandparent->right;
    if(uncle && uncle->color == RED) {
      grandparent->color = RED;
      parent->color = uncle->color = BLACK;
      node = grandparent;
      continue;
    }
```

The next case is if the uncle is black or NULL, then rotate to the right around the grandparent.  

The code looks like the following
```c
  if(parent->right == node)
    rotate_left(parent, NULL);
  rotate_right_and_swap_colors(grandparent, root);
  break;
```

If the node being inserted is to the right of the parent, it needs rotated to the left to keep
one child to either side after the rotation.  If the left rotate isn't done first, the following will happen

```bash
$ $stla/bin/tree_operations FDGB
F1
| \
D2 G2
|    
(B2)

(i)nsert, (e)rase, (r)ight_rotate, (l)eft_rotate, (R)ed, (b)lack, re(c)olor, (h)elp, (q)uit
i C
F1
| \
D2 G2
|    
(B2)
    \
     (C2)

(B2) has a red right child and is red
(C2) has a red parent and is red

(i)nsert, (e)rase, (r)ight_rotate, (l)eft_rotate, (R)ed, (b)lack, re(c)olor, (h)elp, (q)uit
r D
F1
| \
|  G2
|    
B2
  \
   (D2)
   |   
   (C2)

(C2) has a red parent and is red
(D2) has a red left child and is red
```

This just created the inverse problem (D is to right and C is to the left of D).  

Notice that while swapping colors during the rotate, that the placement of the black node doesn't change. This maintains the proper black height.

After inserting A, do a right rotation on its grandparent D because its uncle is NULL (NULL is black).

```bash
$ $stla/bin/tree_operations FDGB
F1
| \
D2 G2
|    
(B2)

(i)nsert, (e)rase, (r)ight_rotate, (l)eft_rotate, (R)ed, (b)lack, re(c)olor, (h)elp, (q)uit
i A
F1
| \
D2 G2
|    
(B2)
|   
(A2)

(A2) has a red parent and is red
(B2) has a red left child and is red

(i)nsert, (e)rase, (r)ight_rotate, (l)eft_rotate, (R)ed, (b)lack, re(c)olor, (h)elp, (q)uit
r D
F1
| \
|  G2
|    
B2
| \
|  (D2)
|      
(A2)
```

If Br was the right child of the parent (Ar), then do an extra left rotation on Ar to make the tree look like the case above before doing the right rotation (there's no need for color swapping because they're both red).

```bash
Starting with a valid red black tree
====================================
F1
| \
D2 G2
|    
(B2)


Operation: insert C
F1
| \
D2 G2
|    
(B2)
    \
     (C2)

(B2) has a red right child and is red
(C2) has a red parent and is red

Operation: left_rotate B
F1
| \
D2 G2
|    
(C2)
|   
(B2)

(B2) has a red parent and is red
(C2) has a red left child and is red

Operation: right_rotate D
F1
| \
|  G2
|    
C2
| \
|  (D2)
|      
(B2)
```

As stated before the else block is where the parent is the right child and the uncle is the left child.  All of the logic is reversed (left is swapped for right).

To recap

1. Link a node into the tree just like you would with a binary search tree
2. Paint the node red
3. Start a forever loop
4. If the node doesn't have a parent, paint the node black and return
5. If the node's parent is black, return
6. If the node's uncle is not NULL and it is red
   a. paint the parent and the uncle black
   b. paint the grandparent red
   c. set the node to be the grandparent and continue in loop (3)
7. If the node is on the same side of the parent as the uncle is to the grandparent, rotate away from the uncle around the parent.
8. Rotate towards the uncle around the grandparent, swap colors with the parent and the grandparent, and return.

## Erase

Erasing nodes in a red black tree is more complex than insertion.  In writing this and trying to visualize what is happening, I needed to keep reminding myself of the importance of maintaining black height and to a lesser extent the other rules (particularly that a node with only one child must have a red child).  Like insertion, node_erase calls a function to fix the balance of the tree once the node is removed.  A key difference is that the color doesn't have to always fixed.  Another difference is that what needs fixed are either the parent or the successor.

The function replace_node_with_child sets the child color to the node that it is replacing's color.
```c
child->color = node->color;
```

If there is one child or if the node is the last node in the tree, the color doesn't need fixed.  The only case in the block below where a node needs fixed is if you erase a black node without any children that isn't the root.

```c
static void fix_color_for_erase(node_t *parent, node_t *node, node_t **root);

bool node_erase(node_t *node, node_t **root) {
  node_t *parent = node->parent;
  if(!node->left) {
    if(node->right)
      replace_node_with_child(node->right, node, root);
    else {
      if(parent) {
        if(parent->left == node)
          parent->left = NULL;
        else
          parent->right = NULL;
        if(node->color == BLACK)
          fix_color_for_erase(parent, NULL, root);
      }
      else
        *root = NULL;
    }
  }
  else if(!node->right)
    replace_node_with_child(node->left, node, root );
```

If you consider the following examples, it should be clear why erasing a red leaf is never a problem or a node with one child.

The tree_operations has a -q feature which is less verbose (you don't get the menu or the list of operations that lead to a successful red black tree structure).

```bash
$ $stla/bin/tree_operations -q ABC
B1
| \
|  (C1)
|      
(A1)

e A
B1
  \
   (C1)

The above tree is a valid red black tree

e C
B1

The above tree is a valid red black tree
```

```bash
$ $stla/bin/tree_operations -q ABC
B1
| \
|  (C1)
|      
(A1)

e A
B1
  \
   (C1)

The above tree is a valid red black tree

e B
C1

The above tree is a valid red black tree
```

The case where fixing the color is important is shown below.  A has no children and is black.
```bash
$ $stla/bin/tree_operations -q ABCD
B1
| \
A2 C2
     \
      (D2)

e A
B1
  \
   C2
     \
      (D2)

B1 has one right child and it isn't red
```

The rest of the erase method is to consider cases where the node being erased has two children.  If the successor is to the right (it doesn't have any left children), it will replace the node to erase as usual.  If the successor has a right child, then its child color will change to black (from red).  We can be assured that the successor's right child is red because it is an only child.

```c
else {
  node_t *successor = node->right;
  if(!successor->left) {
    size_t color = successor->color;
    replace_node_with_child(successor, node, root);
    successor->left = node->left;
    successor->left->parent = successor;
    if(successor->right)
      successor->right->color = BLACK;
    else {
      if(color == BLACK)
        fix_color_for_erase(successor, NULL, root);
    }
  }
```

An example where the successor has a right node.  Erasing F, G becomes successor, G has right child H.  

- Move G into F's spot.
- color G's right (H) black

```bash
$ $stla/bin/tree_operations -q ABCDEFGH
D1
| \
|  (F1)
|  |   \
|  E2   G2
|         \
(B1)       (H2)
|   \          
A2   C2

e F
D1
| \
|  (G1)
|  | \
|  E2 (H1)
|         
(B1)
|   \
A2   C2

(G1) has a red right child and is red
(H1) has a red parent and is red
(H1) has a different black height than A2

b H
D1
| \
|  (G1)
|  |   \
|  E2   H2
|         
(B1)
|   \
A2   C2

The above tree is a valid red black tree
```

An example where the successor is black.  In this case, B is erased, C is the successor and is black.  C is colored the color of B.  The tree becomes invalid because C has a single black child (single children must be red).  The color must be fixed.

```bash
$ $stla/bin/tree_operations -q ABCDEFGH
D1
| \
|  (F1)
|  |   \
|  E2   G2
|         \
(B1)       (H2)
|   \          
A2   C2

e B
D1
| \
|  (F1)
|  |   \
|  E2   G2
|         \
(C1)       (H2)
|              
A2

(C1) has one left child and it isn't red
```

Finally, an example where the successor is red.
```bash
$ $stla/bin/tree_operations -q ABC
B1
| \
|  (C1)
|      
(A1)

e B
C1
|
(A1)

The above tree is a valid red black tree
```

If the successor is to the left of the node to the right of the node to erase, then the normal erasing happens.  If the successor has a child, it would be right and red (because it would be the only child).  In this case, we replace the node to erase with the successor and the successor with the right child of the successor (in both cases, exchanging colors).  The node to the right of the successor will change to black because the successor started as black (because it only had one red child).

```c

    else {
      while(successor->left)
        successor = successor->left;

      size_t color = successor->color;
      node_t *right = successor->right;
      node_t *parent = successor->parent;
      parent->left = right;
      if(right) {
        right->color = BLACK;
        right->parent = parent;
        color = RED;
      }
      replace_node_with_child(successor, node, root);
      successor->left = node->left;
      successor->left->parent = successor;
      successor->right = node->right;
      successor->right->parent = successor;
      if(color == BLACK)
        fix_color_for_erase(parent, NULL, root);
    }
  }
  return true;
}
```

In the example below H is erased and replaced with its successor (I).  I has a right child M which becomes a child of N and is colored black.

```bash
$ $stla/bin/tree_operations -q ACBFHEGPNIONM
F1
| \
|  H2
|  | \
|  G3 (N2)
|     |   \
B2    I3   P3
| \     \  |
A3 C3    | (O3)
     \   |     
      |  (M3)
      |      
      (E3)

e H
F1
| \
|  I2
|  | \
|  G3 (N2)
|     |   \
B2    (M2) P3
| \        |
A3 C3      (O3)
     \         
      (E3)

(M2) has a red parent and is red
(M2) has a different black height than A3
(N2) has a red left child and is red

b M
F1
| \
|  I2
|  | \
|  G3 (N2)
|     |   \
B2    M3   P3
| \        |
A3 C3      (O3)
     \         
      (E3)

The above tree is a valid red black tree
```

In the example below, H is erased (which is black) and replaced with I which doesn't have a child.  The color must be fixed.

```bash
$ $stla/bin/tree_operations -q ACBFHEGPNION
F1
| \
|  H2
|  | \
|  G3 (N2)
|     |   \
B2    I3   P3
| \        |
A3 C3      (O3)
     \         
      (E3)

e H
F1
| \
|  I2
|  | \
|  G3 (N2)
|         \
B2         P3
| \        |
A3 C3      (O3)
     \         
      (E3)

(N2) has one right child and it isn't red
```

In the example below, N is erased (which is red) and replaced with O which doesn't have a right child.  Because N is red, O will be red and the overall tree height will be maintained.  The red black tree properties remain valid.
```bash
$ $stla/bin/tree_operations -q ACBFHEGPNION
F1
| \
|  H2
|  | \
|  G3 (N2)
|     |   \
B2    I3   P3
| \        |
A3 C3      (O3)
     \         
      (E3)

e N
F1
| \
|  H2
|  | \
|  G3 (O2)
|     |   \
B2    I3   P3
| \          
A3 C3
     \
      (E3)

The above tree is a valid red black tree
```

## Quick recap of when tree becomes invalid
- The node to erase has no children, is black, and has a parent.  The node's parent is invalid.
- The node to erase has two children, the successor was originally black, and the successor did not have a right child.  The successor's original parent is invalid (this is the successor itself when the successor is to the right of the node_to_erase, since it replaced its parent).

It's worth noting that when you erase a node and use a successor, that you are really erasing the successor and putting it in place of the node to erase.  That's why in the first case, the parent of the node to erase is used and in the second case, the parent of the successor is used.

## fix_color_for_erase

```c
static void fix_color_for_erase(node_t *parent, node_t *node, node_t **root) {
  node_t *sibling;
  if(parent->right != node) {
    sibling = parent->right;
    if(sibling->color == RED) {
      rotate_left(parent, root);
      sibling = parent->right;
    }
    if(sibling->left) {
      if(sibling->right) {
        if(sibling->right->color == RED) {
          sibling->right->color = BLACK;
          rotate_left(parent, root);
        }
        else {
          if(sibling->left->color == RED) {
            rotate_right(sibling, root);
            rotate_left(parent, root);
            sibling->color = BLACK;
          }
          else {
            sibling->color = RED;
            if(parent->parent && parent->color == BLACK)
              fix_color_for_erase(parent->parent, parent, root);
            else
              parent->color = BLACK;
          }
        }
      }
      else {
        rotate_right(sibling, root);
        rotate_left(parent, root);
        sibling->color = BLACK;
      }
    } else if(sibling->right) {
      sibling->right->color = BLACK;
      rotate_left(parent, root);
    } else {
      sibling->color = RED;
      if(parent->parent && parent->color == BLACK)
        fix_color_for_erase(parent->parent, parent, root);
      else
        parent->color = BLACK;
    }
  }
  else {
    sibling = parent->left;
    if(sibling->color == RED) {
      rotate_right(parent, root);
      sibling = parent->left;
    }
    if(sibling->right) {
      if(sibling->left) {
        if(sibling->left->color == RED) {
          sibling->left->color = BLACK;
          rotate_right(parent, root);
        }
        else {
          if(sibling->right->color == RED) {
            rotate_left(sibling, root);
            rotate_right(parent, root);
            sibling->color = BLACK;
          }
          else {
            sibling->color = RED;
            if(parent->parent && parent->color == BLACK)
              fix_color_for_erase(parent->parent, parent, root);
            else
              parent->color = BLACK;
          }
        }
      }
      else {
        rotate_left(sibling, root);
        rotate_right(parent, root);
        sibling->color = BLACK;
      }
    } else if(sibling->left) {
      sibling->left->color = BLACK;
      rotate_right(parent, root);
    } else {
      sibling->color = RED;
      if(parent->parent && parent->color == BLACK)
        fix_color_for_erase(parent->parent, parent, root);
      else
        parent->color = BLACK;
    }
  }
}
```

The fix_color_for_erase function




[Table of Contents](README.md)  - Copyright 2019 Andy Curtis
