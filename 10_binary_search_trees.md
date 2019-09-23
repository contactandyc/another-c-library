[Table of Contents](README.md)  - Copyright 2019 Andy Curtis

# Binary Search Trees
## The basic structure


A binary search tree is similar to a linked list in that nodes are linked together.  The difference is that each node has two children.  The children are generally understood as being left and right where the left is less than the given node and the right is greater.  Trees in computer science generally grow from a root down (unlike trees in nature which grow up from the root).  This is likely due to the fact that in a binary search tree, the root is considered the top of the tree.  In binary search trees, each child node is aware of its parent node.  In the example below, B is aware of D as a parent, A as a left child, and C as a right child.

```
                D
              /   \
             B     H
            / \   / \
           A   C G   I
                /
               E
                 \
                   F     
```

In the example above, the root is D and the left of D is B, right is H.  Each node refers to up to three other nodes.  For example, the B node refers to D, A, and C.  D is B's parent.  A is B's left child and C is B's right child.  Before continuing too much with theory, we will build out a binary search tree.  Just like before, the only files that will be changed are doubly_linked_list.c will be renamed binary_search_tree.c and the Makefile will have DATA_STRUCTURE defined as binary_search_tree.  The binary search tree is much simpler than the red black tree, but it isn't entirely without complexity.

The code that follows can be found in illustrations/10_binary_search_trees/1_binary_search_tree

First, let's define the structure of the node.
```c
typedef struct node_s {
  struct node_s *parent;
  struct node_s *left;
  struct node_s *right;
  char key;
} node_t;
```

For a binary search tree, instead of thinking in next and previous, we think in terms of two children and a parent.  The children are considered left and right.  We could name the nodes left_child and right_child and that would indeed be a more precise name.  However, left and right have become common names of the left and right children, so we will use left and right.   A linked list can be sorted if the insert method inserts the node in the right place to maintain a sorted list.  It doesn't have to be and indeed it frequently is not sorted.  The binary search tree is sorted.  In a binary search tree, the root might not be the first element in the list of nodes (if you are considering sort order).  In the example above, D is the root, but A would be the first node in the sorted set.  Linked lists always maintain that the root node is the first node in the list.  In a binary search tree, it is rare that the root node is the first node within the sorted set.  It only happens when the root is the left most node in the tree.

A binary search tree can be empty, much like a linked list can be empty.  If it is empty, it doesn't have any nodes.  Just like a linked list, where you need to create a reference to some part of the list (typically the beginning), you need to maintain a link to the root of the tree.  This is just a pointer to the current root node which can change depending upon your tree operations.  Initializing the root node to NULL, indicates that the tree is empty.

```c
node_t *root = NULL;
```

To build the tree, we will need functionality to insert new nodes.  For now, we will assume that our tree maintains unique keys (no duplicate letters).  In a binary search tree insert method, you first find the place where the node would go, and then if it doesn't already exist, you add the node.  It is often a good idea to search for the node prior to inserting.  This will eliminate the need to create and destroy the node needlessly, if it already existed.  It also is easier to understand the find method, so it comes next.

# Find

```c
node_t *node_find(char key, node_t *root) {
  while(root) {
    if(key < root->key)
      root = root->left;
    else if(key > root->key)
      root = root->right;
    else
      return root;
  }
  return NULL;
}
```

You may notice in the code above that the root is no longer the root once it is assigned to its left or right node.  In a binary search tree, you can call the find method with any node in the tree and it will search that node and all of its children.  This fact generally isn't useful, but it is good to understand it as you think about how binary search trees work.  The node_find method above can be converted to a recursive function.

```c
node_t *node_find(char key, node_t *root) {
  if(key < root->key)
    return node_find(key, root->left);
  else if(key > root->key)
    return node_find(key, root->right);
  else
    return root;
}
```

The recursive method requires less code, but is less efficient as the recursion requires more stack manipulation.  Recursive functions are extremely powerful, but when they can be represented as a non-recursive method that is more efficient, it is usually best to use the non-recursive method.

# Insert

The node_insert method will require a pointer to the root pointer so that it can potentially modify what the root is. Insert works much like find, except that it needs to find the spot where the new node can fit.  To find that spot, the parent of that spot must be saved and the actual spot itself.  Because the spot where the new node can fit will be NULL, you can use the parent pointer from a given node.  Once a place is found, the node to insert's parent is to set to the saved parent, the left and right pointers are set to NULL, and the spot is set to reference the node.
```c
#include <stdbool.h>

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

## First, Last, Next, Previous

Before getting to erasing nodes, I'm going to cover basic iteration.  Iteration is needed to solve one case of erasing nodes, so we will cover iteration first.

To find the first node in a tree, you simply traverse all of the left pointers until there are no more.  
```c
node_t *node_first(node_t *n) {
  if (!n)
    return NULL;
  while (n->left)
    n = n->left;
  return n;
}
```

Similarly, to find the last node in a tree, you traverse all of the right pointers until there are no more.
```c
node_t *node_last(node_t *n) {
  if (!n)
    return NULL;
  while (n->right)
    n = n->right;
  return n;
}
```

To find the next node, consider if the current node has a right node.  If it does, the find the left most node from the right node.  Otherwise find the first parent where the child is on the left.
```c
node_t *node_next(node_t *n) {
  if (n->right) {
    n = n->right;
    while (n->left)
      n = n->left;
    return n;
  }
  while (n->parent && n->parent->right == n)
    n = n->parent;
  return n->parent;
}
```

To find the previous node, swap left and right from node_next.  Also note that previous is supported.
```c
bool node_previous_supported() { return true; }

node_t *node_previous(node_t *n) {
  if (n->left) {
    n = n->left;
    while (n->right)
      n = n->right;
    return n;
  }
  while (n->parent && n->parent->left == n)
    n = n->parent;
  return n->parent;
}
```

## Erase

Erasing nodes is a bit more complicated than insertion because the tree may need to be shuffled a bit for nodes which have children.  Consider our example from before.

```
                D
              /   \
             B     H
            / \   / \
           A   C G   I
                /
               E
                 \
                   F     
```

Erasing leaf nodes (nodes without any children are easy).  A, C, F, and I can be erased by simply removing the link from their parent to them.  Erasing nodes which have only one child are relatively simple.  You just replace the child.  To erase E, you would replace E with F.  Replacing nodes with two children isn't too difficult.  Consider removing H.  If you replaced H with I, the tree would still be in tact.  Replacing D is a bit more challenging as H has two children, and if you replaced D with H, how could H point to B, I, and G.  It isn't that big of a problem.  If you reconsider what happened when replacing H with I, you might discover that you were replacing H with the next inorder value (which would be I).  If we look for the next inorder value after D, you would see that it is E.  E has a child F, so F will first take E's place.  Next E can take D's place.  The rules are ...

```
1.  If a node is a leaf, unlink it from it parent
2.  If a node only has one child, replace the node with the child
3.  If a node has two children, find the next inorder node and unlink it first
    using rules 1 and 2.  Next, replace the node with the node that was just
    unlinked.
```

```c
bool node_erase(node_t *node_to_erase, node_t **root) {
  if(node_to_erase->left) {
    if(node_to_erase->right) {
      // case 3
      node_t *next = node_next(node_to_erase);
      node_erase(next, root);
      if(node_to_erase->parent) {
        if(node_to_erase->parent->left == node_to_erase)
          node_to_erase->parent->left = next;
        else
          node_to_erase->parent->right = next;
      }
      else
        *root = next;

      next->parent = node_to_erase->parent;
      next->left = node_to_erase->left;
      next->right = node_to_erase->right;
      next->left->parent = next;
      next->right->parent = next;
    }
    else {
      // case 2
      node_to_erase->left->parent = node_to_erase->parent;
      if(node_to_erase->parent) {
        if(node_to_erase->parent->left == node_to_erase)
          node_to_erase->parent->left = node_to_erase->left;
        else
          node_to_erase->parent->right = node_to_erase->left;
      }
      else
        *root = node_to_erase->left;           
    }
  }
  else if(node_to_erase->right) {
    // case 2
    node_to_erase->right->parent = node_to_erase->parent;
    if(node_to_erase->parent) {
      if(node_to_erase->parent->left == node_to_erase)
        node_to_erase->parent->left = node_to_erase->right;
      else
        node_to_erase->parent->right = node_to_erase->right;
    }
    else
      *root = node_to_erase->right;     
  }
  else {
    // case 1
    if(node_to_erase->parent) {
      if(node_to_erase->parent->left == node_to_erase)
        node_to_erase->parent->left = NULL;
      else
        node_to_erase->parent->right = NULL;
    }
    else
      *root = NULL;    
  }
  return true;
}
```

The above code could be improved.  First, there is some repeated code.
```c
if(node_to_erase->parent) {
  if(node_to_erase->parent->left == node_to_erase)
    node_to_erase->parent->left = NULL;
  else
    node_to_erase->parent->right = NULL;
}
else
  *root = NULL;
```

and
```c
if(node_to_erase->parent) {
  if(node_to_erase->parent->left == node_to_erase)
    node_to_erase->parent->left = node_to_erase->right;
  else
    node_to_erase->parent->right = node_to_erase->right;
}
else
  *root = node_to_erase->right;     
```

and
```c
node_to_erase->left->parent = node_to_erase->parent;
if(node_to_erase->parent) {
  if(node_to_erase->parent->left == node_to_erase)
    node_to_erase->parent->left = node_to_erase->left;
  else
    node_to_erase->parent->right = node_to_erase->left;
}
else
  *root = node_to_erase->left;
```

and
```c
if(node_to_erase->parent) {
  if(node_to_erase->parent->left == node_to_erase)
    node_to_erase->parent->left = next;
  else
    node_to_erase->parent->right = next;
}
else
  *root = next;

next->parent = node_to_erase->parent;
```

are very similar. The only difference is whether NULL, node_to_erase->right, node_to_erase->left, and next get's assigned.

The following function could simplify the code.
```c
static inline void replace_node_with_child(node_t *child, node_t *node, node_t **root ) {
  node_t *parent = node->parent;
  if(parent) {
    if(parent->left == node)
      parent->left = child;
    else
      parent->right = child;
  }
  else
    *root = child;
  if(child)
    child->parent = parent;
}
```

The new node_erase would look like
```c
bool node_erase(node_t *node_to_erase, node_t **root) {
  if(node_to_erase->left) {
    if(node_to_erase->right) {
      // case 3
      node_t *next = node_next(node_to_erase);
      node_erase(next, root);
      replace_node_with_child(next, node_to_erase, root);
      next->left = node_to_erase->left;
      next->right = node_to_erase->right;
      next->left->parent = next;
      next->right->parent = next;
    }
    else
      replace_node_with_child(node_to_erase->left, node_to_erase, root);
  }
  else if(node_to_erase->right)
    replace_node_with_child(node_to_erase->right, node_to_erase, root);
  else
    replace_node_with_child(NULL, node_to_erase, root);

  return true;
}
```

A tiny optimization can be made by not allowing NULL to be passed in as the child.  We can avoid the if(child) check in replace_node_with_child.  

```c
static inline void replace_node_with_child(node_t *child, node_t *node, node_t **root ) {
  node_t *parent = node->parent;
  if(parent) {
    if(parent->left == node)
      parent->left = child;
    else
      parent->right = child;
  }
  else
    *root = child;

  child->parent = parent;
}
```

that means that
```c
else
  replace_node_with_child(NULL, node_to_erase, root);
```

gets replaced with
```c
else {
  node_t *parent = node_to_erase->parent;
  if(parent) {
    if(parent->left == node)
      parent->left = NULL;
    else
      parent->right = NULL;
  }
  else
    *root = NULL;
}
```

Recursion isn't always a bad thing, but it can make code harder to follow.  The remaining code in the node_erase method is ...

```c
node_t *next = node_next(node_to_erase);
node_erase(next, root);
replace_node_with_child(next, node_to_erase, root);
next->left = node_to_erase->left;
next->right = node_to_erase->right;
next->left->parent = next;
next->right->parent = next;
```

How many times will node_erase be called?  Due to how the successor works, it will be exactly one time (because it will be an only child if node_to_erase isn't considered).  I've flipped the cases, so that the easy cases are first.

```c
bool node_erase(node_t *node, node_t **root) {
  if(!node->left) {
    if(node->right) { /* node has one right child */
      replace_node_with_child(node->right, node, root);
    }
    else { /* node has no children, unlink from parent */
      node_t *parent = node->parent;
      if(parent) {
        if(parent->left == node)
          parent->left = NULL;
        else
          parent->right = NULL;
      }
      else { /* no children, no parent, tree is now empty */
        *root = NULL;
      }
    }
  } else if(!node->right) { /* node has one left child */
    replace_node_with_child(node->left, node, root );
  } else { /* node has left and right child */
    // remaining code...
  }
  return true;
}
```

The remaining code will find the successor (or the next inorder node).
1. If the successor is to the right of the node_to_erase, then alter the successor's parent to be the same as the node and link successor's left to the node's left.
2. Otherwise, the successor will be to the left of the node_to_erase's right node. Link the successor's parent's left to the successor's right child.  If the successor's right child is not NULL, link it to the successor's parent.  Fully replace the node_to_erase with the successor.

```c
node_t *successor = node->right;
if(!successor->left) { /* successor is to the right of node */
  replace_node_with_child(successor, node, root);
  successor->left = node->left;
  successor->left->parent = successor;
}
else {
  while(successor->left)
    successor = successor->left;

  successor->parent->left = successor->right;
  if(successor->right)
    successor->right->parent = successor->parent;

  /* replace node with successor */
  replace_node_with_child(successor, node, root);
  successor->left = node->left;
  successor->left->parent = successor;
  successor->right = node->right;
  successor->right->parent = successor;
}
```

The final node_erase looks like
```c
static inline void replace_node_with_child(node_t *child, node_t *node, node_t **root ) {
  node_t *parent = node->parent;
  if(parent) {
    if(parent->left == node)
      parent->left = child;
    else
      parent->right = child;
  }
  else
    *root = child;

  child->parent = parent;
}

bool node_erase(node_t *node, node_t **root) {
  if(!node->left) {
    if(node->right) { /* node has one right child */
      replace_node_with_child(node->right, node, root);
    }
    else { /* node has no children, unlink from parent */
      if(node->parent) {
        node_t *parent = node->parent;
        if(parent->left == node)
          parent->left = NULL;
        else
          parent->right = NULL;
      }
      else { /* no children, no parent, tree is now empty */
        *root = NULL;
      }
    }
  } else if(!node->right) { /* node has one left child */
    replace_node_with_child(node->left, node, root );
  } else { /* node has left and right child */
    node_t *successor = node->right;
    if(!successor->left) { /* successor is to the right of node */
      replace_node_with_child(successor, node, root);
      successor->left = node->left;
      successor->left->parent = successor;
    }
    else {
      while(successor->left)
        successor = successor->left;

      successor->parent->left = successor->right;
      if(successor->right)
        successor->right->parent = successor->parent;

      /* replace node with successor */
      replace_node_with_child(successor, node, root);
      successor->left = node->left;
      successor->left->parent = successor;
      successor->right = node->right;
      successor->right->parent = successor;
    }
  }
  return true;
}
```


Postorder iteration means to iterate through a tree visiting the left and the right nodes before each root.  Postorder starts from the left deepest node of the tree and then iterates through children before parents.  Given the following example, the iteration will look like.

```
        D9
     /      \
    B3       H8
  /   \    /   \
 A1    C2 G6     I7
         /
        E5
          \
           F4
```

or

```
ACBEGFD
```

```
Start with the left deepest node which is A.
Since A is the left child of B, look for the left deepest node under C which is C
Since C is the right child of B, B is next
Since D is the left child of B, look for the left deepest node under H which is F
Since F is the right child of E, E is next
Since G is the left child of G and there isn't a right child, G is next
Since G is the left child of H, look for the left deepest node under I which is I
Since I is the right child of H, H is next
Since H is the right child of D, D is next
Since D has no parent, we are done
```

The left deepest node is where you always choose a child if it exists preferring the left child over the right.  Because this is an internal function, it is safe to assume that (n) will not be NULL.  The function is made static because it is only used within the c file and inline for potential performance.  For example, F is the left deepest node of H.  If a node has no children, that node is returned.
```c
static inline node_t *left_deepest_node(node_t *n) {
  while(true) {
    if(n->left)
      n = n->left;
    else if(n->right)
      n = n->right;
    else
      return n;
  }
}
```

In explaining the traversal above, there are 4 patterns that exist.
```
1.  Start with left deepest node
2.  If node is a left child of parent and there is a right child, look for the
    left deepest node under right child, otherwise use parent.
3.  If node is right child, use parent
4.  If node has no parent, we are done
```

Statements 2-4 are used for finding next nodes in postorder iteration.  Statement 3 and statement 4 are actually the same in as much as every node has a parent, it just may be that the parent is NULL.  The otherwise element of statement 2 also returns the parent.  The first part of statement 2 is the only case where something other than the parent is returned.  Given this, we can state the following about find next nodes in postorder iteration.

```
If node's parent is not NULL and the node is a left child of its parent and
there is a right child, return the left deepest node of the right child of the
node's parent, otherwise return the node's parent.
```

If a tree is destroyed using postorder iteration, all of the children are destroyed before parents, so the tree remains in tact throughout the destruction.  The node_first_to_erase (or first in postorder iteration) calls the left_deepest_node method above if n is not NULL.

```c
node_t *node_first_to_erase(node_t *n) {
  if (!n)
    return NULL;
  return left_deepest_node(n);
}
```

The node_next_to_erase (or next in postorder iteration) uses our statement above.
```c
node_t *node_next_to_erase(node_t *n) {
  node_t *parent = n->parent;
  if (parent && n == parent->left && parent->right)
    return left_deepest_node(parent->right);
  else
    return parent;
}
```

The last function to build is a method to print the tree.  It presented a different kind of challenge that programmers often face and so I'm separating it out.  In order to print the binary search tree, I've implemented an approach that requires copying the tree into a different structure.  Because we have built the stla_allocator and the stla_pool.  I've decided to go ahead and use them.  To demonstrate how they can be useful, I've commented out the free and stla_pool_destroy calls.  Before getting into how to print, let's review the changes to test_data_structure.c and data_structure.h.

data_structure.h includes "stla_pool.h"
```c
#include "stla_pool.h"
```

and node_print takes a pointer to stla_pool_t *pool
```c
void node_print(node_t *root);
```

becomes
```c
void node_print(stla_pool_t *pool, node_t *root);
```

The test_data_structure.c needs to incorporate the pool.

In the main function, the pool needs initialized and destroyed.  Also, the free call gets changed to stla_free.  I've commented the free and destroy call on purpose.
```c
stla_pool_t *pool = stla_pool_init(1024);
char *arg = get_valid_characters(argv[1]);
test_data_structure(pool, arg, repeat);
// stla_free(arg);
// stla_pool_destroy(pool);
```

get_valid_characters has a call to strdup which changes from
```c
char *res = strdup(p);
```

to
```c
char *res = stla_strdup(p);
```

The test_data_structure definition changes from
```c
void test_data_structure(const char *arg, int repeat) {
```

to
```c
void test_data_structure(stla_pool_t *pool, const char *arg, int repeat) {
```

All of the calls to node_print now take the pool parameter.
```c
node_print(root);
```

becomes
```c
node_print(pool, root);
```

The Makefile also includes the src library to incorporate the stla_pool object.

The following code is found in <i>illustrations/10_binary_search_trees/1_binary_search_tree</i>
```bash
cd $stla/illustrations/10_binary_search_trees/1_binary_search_tree
```

```bash
$ make
gcc -g -O3 -I../../../src -D_STLA_DEBUG_MEMORY_=NULL ../../../src/stla_timer.c ../../../src/stla_allocator.c ../../../src/stla_buffer.c ../../../src/stla_pool.c test_data_structure.c binary_search_tree.c -o test_data_structure -DDATA_STRUCTURE=\"binary_search_tree\"
Creating binary_search_tree for PDCBAEMLOQTRYZ
P1
| \
|  Q2
|    \
D2    T3
| \   | \
C3 |  R4 Y4
|  |       \
B4 E3       Z5
|    \        
A5    M4
      | \
      L5 O5

print_using_iteration: ABCDELMOPQRTYZ
print_using_reverse_iteration: ZYTRQPOMLEDCBA

2527 byte(s) allocated in 17 allocations (680 byte(s) overhead)
test_data_structure.c:244 [stla_pool] size: 1050, max_size: 1050, initial_size: 1024 used: 2152
test_data_structure.c:204: 15
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
../../../src/stla_pool.c:121: 1040
```

Notice that just before the program ended, that it printed 17 lines where allocations happened.  These are allocations which were not properly freed.  Let's check out the lines around 14 of binary_search_tree.c.

```c
node_t *node_init(char key) {
  node_t *n = (node_t *)stla_malloc(sizeof(node_t));
  n->left = n->right = n->parent = NULL;
  n->key = key;
  return n;
}

void node_destroy(node_t *n) {
  // stla_free(n);
}
```

Notice that the stla_free is commented out.  Let's uncomment it and run make again.

```bash
$ make
gcc -g -O3 -I../../../src -D_STLA_DEBUG_MEMORY_=NULL ../../../src/stla_timer.c ../../../src/stla_allocator.c ../../../src/stla_buffer.c ../../../src/stla_pool.c test_data_structure.c binary_search_tree.c -o test_data_structure -DDATA_STRUCTURE=\"binary_search_tree\"
Creating binary_search_tree for PDCBAEMLOQTRYZ
P1
| \
|  Q2
|    \
D2    T3
| \   | \
C3 |  R4 Y4
|  |       \
B4 E3       Z5
|    \        
A5    M4
      | \
      L5 O5

print_using_iteration: ABCDELMOPQRTYZ
print_using_reverse_iteration: ZYTRQPOMLEDCBA

2079 byte(s) allocated in 3 allocations (120 byte(s) overhead)
test_data_structure.c:244 [stla_pool] size: 1050, max_size: 1050, initial_size: 1024 used: 2152
test_data_structure.c:204: 15
../../../src/stla_pool.c:121: 1040
```

There are three lines left.  Let's consider the second one (test_data_structure.c:204).  The lines around 204 of test_data_structure.c are below.
```c
char *get_valid_characters(const char *p) {
	char *res = stla_strdup(p);
	char *wp = res;
```

Line 204 is a stla_strdup call which ultimately gets returned from get_valid_characters.  If we look for the get_valid_characters call, we find it in the main function.  The arg is not freed (stla_free is commented out).  

```c
stla_pool_t *pool = stla_pool_init(1024);
char *arg = get_valid_characters(argv[1]);
test_data_structure(pool, arg, repeat);
// stla_free(arg);
// stla_pool_destroy(pool);
return 0;
```

Let's uncomment the stla_free(arg); line and run again.
```bash
$ make
gcc -g -O3 -I../../../src -D_STLA_DEBUG_MEMORY_=NULL ../../../src/stla_timer.c ../../../src/stla_allocator.c ../../../src/stla_buffer.c ../../../src/stla_pool.c test_data_structure.c binary_search_tree.c -o test_data_structure -DDATA_STRUCTURE=\"binary_search_tree\"
Creating binary_search_tree for PDCBAEMLOQTRYZ
P1
| \
|  Q2
|    \
D2    T3
| \   | \
C3 |  R4 Y4
|  |       \
B4 E3       Z5
|    \        
A5    M4
      | \
      L5 O5

print_using_iteration: ABCDELMOPQRTYZ
print_using_reverse_iteration: ZYTRQPOMLEDCBA

2064 byte(s) allocated in 2 allocations (80 byte(s) overhead)
test_data_structure.c:244 [stla_pool] size: 1050, max_size: 1050, initial_size: 1024 used: 2152
../../../src/stla_pool.c:121: 1040
```

The test_data_structure program allows us to print multiple binary search trees if we pass a third argument which is the number of random trees to print (in addition to the one that is in order at the end).

```bash
$ ./test_data_structure PDCBAEMLOQTRYZ 2
Creating binary_search_tree for PDCBAEMLOQTRYZ
L1
| \
|  Q2
|  | \
|  |  R3
|  |    \
|  M3    T4
|    \     \
C2    O4    Z5
| \     \   |
|  D3    P5 Y6
|    \        
A3    E4
  \     
   B4

Q1
| \
|  R2
|    \
B2    Y3
| \   | \
A3 |  T4 Z4
   |       
   M3
   | \
   |  P4
   |  |
   |  O5
   |    
   C4
     \
      D5
        \
         E6
           \
            L7

P1
| \
|  Q2
|    \
D2    T3
| \   | \
C3 |  R4 Y4
|  |       \
B4 E3       Z5
|    \        
A5    M4
      | \
      L5 O5

print_using_iteration: ABCDELMOPQRTYZ
print_using_reverse_iteration: ZYTRQPOMLEDCBA

4144 byte(s) allocated in 4 allocations (160 byte(s) overhead)
test_data_structure.c:244 [stla_pool] size: 3150, max_size: 3150, initial_size: 1024 used: 4232
../../../src/stla_pool.c:121: 1040
../../../src/stla_pool.c:121: 1040
../../../src/stla_pool.c:121: 1040
```

Notice that since we printed 3 trees, the total bytes allocated increased to 4140 bytes.  Let's review the test_data_structure function for a moment.

```bash
void test_data_structure(stla_pool_t *pool, const char *arg, int repeat) {
  printf("Creating %s for %s\n", DATA_STRUCTURE, arg);
  node_t *root;
  for( int i=0; i<repeat; i++ ) {
    // stla_pool_clear(pool);
    root = fill_data_structure_randomly(arg);
    node_print(pool, root);
    find_everything(arg, root);
    find_and_erase_everything(arg, root);
  }
  // stla_pool_clear(pool);
  root = fill_data_structure(arg);
  node_print(pool, root);
  print_using_iteration(root);
  if (node_previous_supported())
    print_using_reverse_iteration(root);
  destroy_using_iteration(root);
  printf("\n");
}
```

For each call of node_print, the pool uses more memory.  However, we can clear the pool just after each print since the pool is only used for allocation within the node_print call.  More specifically, we can clear the pool at the beginning of the for loop to repeat building, printing, and destroying the tree, and just before we recreate the tree one last time.  If you uncomment the stla_pool_clear calls, you can go ahead and rebuild.

Rebuild
```bash
make
```

and run again
```bash
$ ./test_data_structure PDCBAEMLOQTRYZ 2
.
.
.
2064 byte(s) allocated in 2 allocations (80 byte(s) overhead)
test_data_structure.c:246 [stla_pool] size: 1050, max_size: 1050, initial_size: 1024 used: 2064
../../../src/stla_pool.c:121: 1040
```

You should see that the memory usage didn't grow this time.  If we uncomment the stla_pool_destroy call in the main function, we will see the following output.

```bash
$ make
gcc -g -O3 -I../../../src -D_STLA_DEBUG_MEMORY_=NULL ../../../src/stla_timer.c ../../../src/stla_allocator.c ../../../src/stla_buffer.c ../../../src/stla_pool.c test_data_structure.c binary_search_tree.c -o test_data_structure -DDATA_STRUCTURE=\"binary_search_tree\"
Creating binary_search_tree for PDCBAEMLOQTRYZ
P1
| \
|  Q2
|    \
D2    T3
| \   | \
C3 |  R4 Y4
|  |       \
B4 E3       Z5
|    \        
A5    M4
      | \
      L5 O5

print_using_iteration: ABCDELMOPQRTYZ
print_using_reverse_iteration: ZYTRQPOMLEDCBA
```

There are no longer any memory leaks and the pool is being used efficiently.  It's worth noting that sometimes it is useful to comment out the destroy call to see how the pool is performing.

Before moving on, it is worth looking at how unbalanced binary search trees can become.  The simplest way to do it is to put all of the keys in sequence.

For example,
```bash
$ ./test_data_structure abcdefgh
Creating binary_search_tree for abcdefgh
a1
  \
   b2
     \
      c3
        \
         d4
           \
            e5
              \
               f6
                 \
                  g7
                    \
                     h8

print_using_iteration: abcdefgh
print_using_reverse_iteration: hgfedcba
```

or
```bash
$ ./test_data_structure hgfedcba
Creating binary_search_tree for hgfedcba
h1
|
g2
|
f3
|
e4
|
d5
|
c6
|
b7
|
a8

print_using_iteration: abcdefgh
print_using_reverse_iteration: hgfedcba
```

To really understand how to print a binary search tree, I've made a slight modification to the node printing function so that it can print the key a random number of times followed by the depth.

The code that follows can be found in illustrations/10_binary_search_trees/2_binary_search_tree

```bash
cd $stla/illustrations/10_binary_search_trees/2_binary_search_tree
```

```bash
$ make
gcc -g -O3 -I../../../src -D_STLA_DEBUG_MEMORY_=NULL ../../../src/stla_timer.c ../../../src/stla_allocator.c ../../../src/stla_buffer.c ../../../src/stla_pool.c test_data_structure.c binary_search_tree.c -o test_data_structure -DDATA_STRUCTURE=\"binary_search_tree\"
Creating binary_search_tree for PDCBAEMLOQTRYZ
PPPPPPPP1
|        \
DDDDD2    QQQQQ2
|     \         \
|      EEE3      T3
|          \     | \
CCCCCCCCC3  |    |  YYYYYYYYYYYYY4
|           |    |                \
BBBBBBBBB4  |    RRRRRR4           ZZZ5
|           |                          
|           MMMMMMMMMM4
|           |          \
|           LLLLLLLLL5  OOOOOOOOO5
|                                 
AAAAAAAAAAA5

print_using_iteration: ABCDELMOPQRTYZ
print_using_reverse_iteration: ZYTRQPOMLEDCBA
```

In the example above, AAAAAAAAAAA5 is pushed down until LLLLLLLLL5 is printed.  CCCCCCCCC3 is pushed down because it conflict with EEE3.  RRRRRR4 is pushed down because it would conflict with YYYYYYYYYYYYY4.

The way that I have designed the print function is to print the left nodes vertically below the first character of the key and the right nodes using a back slash spaced one character after the parent key.  The algorithm may not be the most optimal solution, but it does seem to work.  Technically, we don't need a print function to be able to use a binary search tree, but they are nice to visualize what is happening.  In addition to this, the solution is interesting and may lead to other solutions.

1. Each node must have a position (horizontal) and that position will not change.  The node may only be pushed down.
2. If a node conflicts with the next node on the same level (or a vertical bar), push the given node down.
3. If a node has a right child, then add two bytes to conflict check
4. Determine the length of each node at the beginning to simplify the computation.

To solve this, the first thing I did is create a copy of the tree which consists of the position of each node, the printed_key, the length of the printed_key, and the depth of the node from its parent (1 if directly beneath parent).  Because we are making a copy of the tree, we also need a parent, left, and right pointer.  I created a number of helper functions to make this work which are all declared as static so they don't conflict with the global namespace.

The structure of the node looks like...
```c
typedef struct node_print_item_s {
  size_t position;
  char *printed_key;
  size_t length;
  int depth;
  struct node_print_item_s *parent;
  struct node_print_item_s *left, *right;
} node_print_item_t;
```

The copy_tree function looks like
```c
static void copy_tree(stla_pool_t *pool, node_t *node,
                      node_print_item_t **res, node_print_item_t *parent ) {
  node_print_item_t *copy = (node_print_item_t *)stla_pool_alloc(pool, sizeof(node_print_item_t));
  *res = copy;

  copy->printed_key = get_printed_key(pool, node);
  copy->length = strlen(copy->printed_key);
  copy->position = parent ? ((parent->left == copy) ? parent->position : parent->position + parent->length + 1) : 0;
  copy->depth = 1;
  copy->left = NULL;
  copy->right = NULL;
  copy->parent = parent;

  if(node->left)
    copy_tree(pool, node->left, &copy->left, copy );
  if(node->right)
    copy_tree(pool, node->right, &copy->right, copy );
}
```

I'm using the pool to allocate each node.  copy_tree is a recursive function which allocates nodes and puts them into the 3rd parameter.  The printed_key is printed using the following function.
```c
static char *get_printed_key(stla_pool_t *pool, node_t *n ) {
  // return stla_pool_strdupf(pool, "%c%d", n->key, get_depth(n));

  int r=rand() % 15;
  char *res = (char *)stla_pool_ualloc(pool, r+4);
  for( int i=0; i<=r; i++ )
    res[i] = n->key;
  sprintf(res+r+1, "%d", get_depth(n));
  return res;
}
```

The normal get_printed_key function would just use the stla_pool_strdupf function and print the character followed by the depth.  The depth is calculated using the following function.
```c
static int get_depth(node_t *n) {
  int depth = 0;
  while (n) {
    depth++;
    n = n->parent;
  }
  return depth;
}
```

All that the depth function does is count how many parents a node has.  The rest of the get_printed_key method is below.  It prints the key from 1 to 15 times and then appends the depth to the key.
```c
int r=rand() % 15;
char *res = (char *)stla_pool_ualloc(pool, r+4);
for( int i=0; i<=r; i++ )
  res[i] = n->key;
sprintf(res+r+1, "%d", get_depth(n));
return res;
```

copy->length is set by simply getting the length of the printed_key.
```c
copy->length = strlen(copy->printed_key);
```

1. If the copy doesn't have a parent, then the position is 0 because it is the root node.
2. If the copy has a parent and the copy is the left child, then the position is the same as the parent's position.
3. Otherwise, the copy is the right child of the parent and the position is the parent's position plus its length + 1.
```c
copy->position = parent ? ((parent->left == copy) ? parent->position : parent->position + parent->length + 1) : 0;
```

Each node is initially a depth of 1 away from its parent.
```c
copy->depth = 1;
```

The left and right pointers are initially NULL and the parent is set to the parent that is passed into the function.
```c
copy->left = NULL;
copy->right = NULL;
copy->parent = parent;
```

If the node has a left, recurse to the left
```c
if(node->left)
  copy_tree(pool, node->left, &copy->left, copy );
```

If the node has a right, recurse to the right
```c
if(node->right)
  copy_tree(pool, node->right, &copy->right, copy );
```

The copy_tree method copies the node_t tree.  

A few helper functions are needed to make the print work.  

The first finds the first parent which is a left parent that also has a right node.
```
   A
    \
     D
     |\
     B E
      \
       C
```

In the above example, if C is the start node, then D would be the first node that is a left parent and that also has a right node (E).  In addition to finding the parent, the depth is summed up along the path to the parent node that is the left parent with a right child.
```c
static node_print_item_t *find_left_parent_with_right_child( node_print_item_t * item,
                                                             int *depth ) {
  while(item->parent && (item->parent->right == item || !item->parent->right)) {
    *depth += item->depth;
    item = item->parent;
  }
  *depth += item->depth;
  return item->parent;
}
```


The next function seeks to find the left most node at or deeper than a given depth.  This will seek from an item until the depth is less than or equal to the depth on the given node.  It recurses to the left first and only seeks to the right only if left doesn't exist.  If the path doesn't reach the given depth, the function returns NULL and through recursion, chooses another path unless none exist (at which point NULL is returned).
```c
static node_print_item_t *find_left_most_at_depth( node_print_item_t * item, int depth ) {
  if(!item)
    return NULL;

  if(depth <= item->depth)
    return item;
  if(item->left) {
    node_print_item_t *r = find_left_most_at_depth(item->left, depth-item->depth);
    if(r)
      return r;
  }
  if(item->right) {
    node_print_item_t *r = find_left_most_at_depth(item->right, depth-item->depth);
    if(r)
      return r;
  }
  return NULL;
}
```

To find the next peer, find the first left parent with a right child and check if there is a left most node at the same level.  If none exists, find the next left parent with a right child (of the current left parent with a right child) and repeat the check for the left most node at the same level.  Repeat until the root is reached (and NULL is returned) or until a left most node is found at the same level.
```c
static node_print_item_t *find_next_peer( node_print_item_t * item, int depth ) {
  while(item) {
    node_print_item_t *p = find_left_parent_with_right_child(item, &depth);
    if(!p)
      return NULL;
    node_print_item_t *np = find_left_most_at_depth( p->right, depth );
    if(np)
      return np;
    item = p;
  }
  return NULL;
}
```

Finally, get_node_depth sums the depth all the way to the root node
```c
static int get_node_depth( node_print_item_t *item ) {
  int r=0;
  while(item) {
    r += item->depth;
    item = item->parent;
  }
  return r;
}
```

Now we are ready to print the binary search tree.  Below is the node_print method which will be followed with it being broken down.
```c
void node_print(stla_pool_t *pool, node_t *root) {
  if (!root)
    return;

  node_print_item_t *printable = NULL;
  copy_tree(pool, root, &printable, NULL );

  node_print_item_t *sn,*n,*n2;
  int actual_depth;
  int depth=1;
  while(true) {
    sn = find_left_most_at_depth(printable, depth);
    if(!sn)
      break;
    int position = 0;
    n = sn;
    while(n) {
      for( ; position<n->position; position++ )
        printf( " ");
      actual_depth=get_node_depth(n);
      if(actual_depth == depth) {
        n2 = find_next_peer(n, 0);
        int extra = 0;
        if(n->right)
          extra = 2;

        if(n2 && (n->position+n->length+1+extra > n2->position)) {
          n->depth++;
          printf( "|");
          position++;
        }
        else {
          printf("%s", n->printed_key);
          position += n->length;
        }
        n = n2;
      }
      else {
        n = find_next_peer(n, depth-actual_depth);
        printf( "|");
        position++;
      }
    }
    printf( "\n");
    position = 0;
    n = sn;
    while(n) {
      for( ; position<n->position; position++ )
        printf( " ");
      actual_depth=get_node_depth(n);
      if(actual_depth == depth) {
        if(n->left) {
          printf( "|" );
          position++;
        }
        for( ; position<n->position+n->length; position++ )
          printf( " ");
        if(n->right) {
          printf( "\\" );
          position++;
        }
      }
      else {
        printf( "|");
        position++;
      }
      n = find_next_peer(n, depth-actual_depth);
    }
    printf( "\n");
    depth++;
  }
}
```

If the root is NULL, there is nothing to print
```c
void node_print(stla_pool_t *pool, node_t *root) {
  if (!root)
    return;
```

The copy_tree expects the root of a tree and a pointer which is initially pointing to NULL.  When copy_tree is complete, printable will be a complete replica of the tree referenced by root.  From this point forward, root will no longer be used.
```c
  node_print_item_t *printable = NULL;
  copy_tree(pool, root, &printable, NULL );
```

A few variables are declared that will be needed during the process of printing the tree.
```c
node_print_item_t *sn,*n,*n2;
int actual_depth;
```

At each level, there must be at one node that can be printed.  If there isn't a left most node at the given depth, we are done printing the tree.  The left most node might be deeper than the current depth.
```c
int depth=1;
while(true) {
  sn = find_left_most_at_depth(printable, depth);
  if(!sn)
    break;

  ...

  depth++;
}
```

sn is the starting node.  Find all of the nodes that are on the same level and print the nodes and vertical bars (as necessary).
1. print spaces until position is equal to n->position
2. get the actual depth of n (n might be below the current level)
3. if the node is at the current depth
   a. find the next peer at the same level.
   b. compute an extra length of 2 if there is a right child of n (one for the backslash and one for the vertical bar if the node needs pushed down).
   c. if there is a next peer and n's position + n's length + 1 + extra is greater than n2's position, push down the current node by increasing its depth and printing a vertical bar (and incrementing position because we wrote 1 byte).
   d. otherwise, print the key and add the length of the key to the position.
   e. set n to n2 (n2 may be NULL)
4. otherwise, the node is deeper than the current depth, find the next peer, print the vertical bar and increment position.  Note that when finding the next peer, the depth we are looking for needs to offset by the difference between the current depth and the actual depth of the node.
5. loop until n is NULL
```c
int position = 0;
n = sn;
while(n) {
  for( ; position<n->position; position++ )
    printf( " ");
  actual_depth=get_node_depth(n);
  if(actual_depth == depth) {
    n2 = find_next_peer(n, 0);
    int extra = 0;
    if(n->right)
      extra = 2;

    if(n2 && (n->position+n->length+1+extra > n2->position)) {
      n->depth++;
      printf( "|");
      position++;
    }
    else {
      printf("%s", n->printed_key);
      position += n->length;
    }
    n = n2;
  }
  else {
    n = find_next_peer(n, depth-actual_depth);
    printf( "|");
    position++;
  }
}
printf( "\n");
```

Restart the loop to print the left and right connection bars.  This is simpler because collisions have already been detected and nodes have been pushed down.  For every character that is printed, position must be incremented.  If a node is at the actual depth, print the left vertical bar if needed, fill in spaces, and print the right backslash if needed.  If the node is below the actual level, simply print the vertical bar.  Finally, find the next peer at the level and repeat until no more peers exist.
```c
position = 0;
n = sn;
while(n) {
  for( ; position<n->position; position++ )
    printf( " ");
  actual_depth=get_node_depth(n);
  if(actual_depth == depth) {
    if(n->left) {
      printf( "|" );
      position++;
    }
    for( ; position<n->position+n->length; position++ )
      printf( " ");
    if(n->right) {
      printf( "\\" );
      position++;
    }
  }
  else {
    printf( "|");
    position++;
  }
  n = find_next_peer(n, depth-actual_depth);
}
printf( "\n");
```

To finalize the work, we will remove the random number of prints in the get_printed_key function.

```c
char *get_printed_key(stla_pool_t *pool, node_t *n ) {
  // return stla_pool_strdupf(pool, "%c%d", n->key, get_depth(n));

  int r=rand() % 15;
  char *res = (char *)stla_pool_ualloc(pool, r+4);
  for( int i=0; i<=r; i++ )
    res[i] = n->key;
  sprintf(res+r+1, "%d", get_depth(n));
  return res;
}
```

becomes

```c
char *get_printed_key(stla_pool_t *pool, node_t *n ) {
  return stla_pool_strdupf(pool, "%c%d", n->key, get_depth(n));
}
```

A quick note on how the stla_pool was useful.  The copy of the tree was constructed and never had to free any of the nodes associated with it.  Both nodes and the printed keys were allocated.  If we didn't have the pool (or something like it), we would have needed to free each node in the tree.  The node_init and node_destroy methods use stla_malloc and stla_free.  If we weren't concerned with destroying the tree or erasing nodes, we could have used the pool for the allocation and never had to erase the tree at all.  A call to stla_pool_clear would make the memory reusable for another purpose (such as building another tree).  Technically, it takes as long to construct a binary search tree as it does to destroy one.  If you use the pool, you can avoid all of the destruction time.  This will be explored further once the red black tree (a balanced binary search tree is complete).

# Quick Recap

- To find the first node in a binary tree, find the left most node from the root.
- To find the last node, find the right most node from the root.
- To find the next node, consider if the current node has a right node.  If it does, the find the left most node from the right node.  Otherwise find the first parent where the child is on the left.
- Finding the previous node is the same as finding the next node if you swap the words left and right in all places.
- To find a given node, recursively choose the left node if the given node is less than the current node (where the left node becomes the current node), the right node if the given node is greater than the current node (where the right node becomes the current node), and if the current node is equal to the given node, return the given node.  If a NULL leaf is reached, the given node doesn't exist in the tree.
- To insert a given node, find where the parent node where the given node should go (similar to find), and link the given node's parent to that parent.  Also, link the parent node's left or right child to the given node.
- Erasing a node is a bit more complex.  It involves finding the successor and replacing it (perhaps look back if you don't understand it).
- Printing a binary search tree is largely challenging because of the need to fit in a two dimensional space.  It involves copying the binary tree into a print friendly structure and pushing down nodes that don't fit.
- The pool can be useful for constructing nodes in a tree, especially if you don't want to worry about deconstructing the tree.

# [Balancing Binary Search Trees](11_balancing_binary_search_trees.md)

[Table of Contents](README.md)  - Copyright 2019 Andy Curtis
