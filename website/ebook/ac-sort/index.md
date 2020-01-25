---
path: "/ac-sort"
posttype: "docs"
title: "ac_sort"
---

```c
#include "ac_sort.h"
```

The ac\_sort is meant to generally be a replacement for the qsort and qsort\_r methods in C.  Like qsort vs qsort\_r, there is a version where the comparison method takes two params and one that takes two params with an arg.  Because ac\_sort is built as a macro, you can define your comparison method as an inline method and define define it when defining the macro to potentially gain a performance boost.  The ac\_sort package is meant to sort fixed length types.  This has the advantage that the compare methods have types in them and the compiler can do a better job of error checking.  If your data doesn't have a type corresponding to it, you can either create one and use casts or just use qsort or qsort\_r.  The macros to produce code have a \_m suffix.  The macros to produce a header definition have a \_def suffix.  Functions and/or header definitions can be made to be static and/or inline by prefixing the macro call with those keywords.

For example, a static inlined function named sort\_names can be created as follows.
```c
#include "ac_sort.h"

#include <string.h>

typedef struct {
  char *name;
} name_t;

static inline int compare_name(const name_t *a, const name_t *b) {
  return strcmp(a->name, b->name);
}

static inline ac_sort_m(sort_names, compare_name, name_t);
```

The function that would be produced would have the following signature.
```c
void sort_names(name_t *base, size_t num_elements);
```

To create the same function in a c file and expose it in a header file, do the following...

source\_code.h
```c
#include "ac_sort.h"

struct name_s;
typedef struct name_s name_t;

ac_sort_def(sort_names, name_t);
```

Notice that the ac\_sort\_def doesn't reference the compare function.  This is because it isn't needed to define the function prototype.  The ac\_sort\_def can be used in the same file as the ac\_sort\_m if it desired to declare the function prototype prior to the function being defined.

source\_code.c
```c
#include "source_code.h"

#include <string.h>

struct name_s {
  char *name;
};

static inline int compare_name(const name_t *a, const name_t *b) {
  return strcmp(a->name, b->name);
}

ac_sort_m(sort_names, compare_name, name_t);
```

The following files are necessary to include in your own package.  You can also just include the whole ac\_ library.  Note that ac\_sort is 100% header files and can easily be included in other packages.

```
ac_sort.h
impl/ac_sort.h
impl/ac_sort/*.h
```

## ac\_sort\_m

```c
ac_sort_m(function_name, compare_function, datatype);
```

ac\_sort\_m is a macro that outputs a sort function named function\_name.  The compare function is called from within the outputted function and it expects to sort data of type datatype.  The compare function should have the following prototype (where datatype is replaced by the datatype specified as the third argument to ac\_sort\_m).

```c
int compare(const datatype *a, const datatype *b);
```

The outputted sort function will have the following prototype (again with datatype being replaced by the specified datatype as the third argument to ac\_sort\_m).

```c
void function_name(datatype *base, size_t num_elements);
```

For example, to create a function named sort\_ints, it might look like the following.

```c
static inline int compare_ints(const int *a, const int *b) {
  if(*a != *b)
    return (*a < *b) ? -1 : 1;
  return 0;
}

ac_sort_m(sort_ints, compare_ints, int);
```

It's generally more efficient to inline the compare function.  This would output the following function.
```c
void sort_ints(int *base, size_t num_elements) {
  // code
}
```

If static prefixed the macro as follows...
```c
static inline int compare_ints(const int *a, const int *b) {
  if(*a != *b)
    return (*a < *b) ? -1 : 1;
  return 0;
}

static ac_sort_m(sort_ints, compare_ints, int);
```

The outputted function would be...
```c
static void sort_ints(int *base, size_t num_elements) {
  // code
}
```

## ac\_sort\_def

```c
ac_sort_def(function_name, datatype);
```

ac\_sort\_def is a macro that produces a function prototype.  The following will produce a prototype for sort\_ints which takes int as a datatype...

```c
#include "ac_sort.h"

ac_sort_def(sort_ints, int);
```

produces
```c
void sort_ints(int *base, size_t num_elements);
```

static and inline modifiers (and any other modifier can be added prior to the macro).
```c
static inline ac_sort_def(sort_ints, int);
```

produces
```c
static inline void sort_ints(int *base, size_t num_elements);
```


## ac\_sort\_compare\_m

```c
ac_sort_compare_m(function_name, datatype);
```

ac\_sort\_compare\_m is similar to ac\_sort\_m except that the compare function is added as an argument to the outputted function.

```c
ac_sort_compare_m(sort_ints, int);
```

produces
```c
void sort_ints(int *base, size_t num_elements,
               int (*compare)(const int *a, const int *b)) {
  // code
}
```

and expects a compare function that looks like...
```c
int compare_ints(const int *a, const int *b) {
  if(*a != *b)
    return (*a < *b) ? -1 : 1;
  return 0;
}
```

See ac\_sort\_m for more details

## ac\_sort\_compare\_def

```c
ac_sort_compare_def(function_name, datatype);
```

ac\_sort\_compare\_def is similar to ac\_sort\_def except that the compare function is added as an argument to the outputted function prototype.

```c
#include "ac_sort.h"

ac_sort_compare_def(sort_ints, int);
```

produces
```c
void sort_ints(int *base, size_t num_elements,
               int (*compare)(const int *a, const int *b));
```

See ac\_sort\_def, ac\_sort\_m for more details.

## ac\_sort\_arg\_m

```c
ac_sort_arg_m(function_name, compare_function, datatype);
```

ac\_sort\_arg\_m is similar to ac\_sort\_m except that it adds a void \*arg which gets passed to the comparison function.  The compare function is also expected to have a void \*arg parameter.

```c
static inline int compare_ints(const int *a, const int *b, void *arg) {
  if(*a != *b)
    return (*a < *b) ? -1 : 1;
  return 0;
}

ac_sort_arg_m(sort_ints, compare_ints, int);
```

produces
```c
void sort_ints(int *base, size_t num_elements, void *arg) {
  // code
}
```

See ac\_sort\_m for more details

## ac\_sort\_arg\_def

```c
ac_sort_arg_def(function_name, datatype);
```

ac\_sort\_arg\_def is similar to ac\_sort\_def except that it adds a void \*arg which gets passed to the comparison function.

```c
#include "ac_sort.h"

ac_sort_arg_def(sort_ints, int);
```

produces
```c
void sort_ints(int *base, size_t num_elements, void *arg);
```

See ac\_sort\_def, ac\_sort\_m for more details.

## ac\_sort\_compare\_arg\_m

```c
ac_sort_compare_arg_m(function_name, datatype);
```

ac\_sort\_compare\_arg\_m is similar to ac\_sort\_arg\_m except that the compare function is added as an argument to the outputted function.

```c
ac_sort_compare_arg_m(sort_ints, int);
```

produces
```c
void sort_ints(int *base, size_t num_elements,
               int (*compare)(const int *a, const int *b, void *arg),
               void *arg) {
  // code
}
```

and expects a compare function that looks like...
```c
int compare_ints(const int *a, const int *b, void *arg) {
  if(*a != *b)
    return (*a < *b) ? -1 : 1;
  return 0;
}
```

See ac\_sort\_m for more details

## ac\_sort\_compare\_arg\_def

```c
ac_sort_compare_arg_def(function_name, datatype);
```

ac\_sort\_compare\_arg\_def is similar to ac\_sort\_arg\_def except that the compare function is added as an argument to the outputted function prototype.

```c
#include "ac_sort.h"

ac_sort_compare_arg_def(sort_ints, int);
```

produces
```c
void sort_ints(int *base, size_t num_elements,
               int (*compare)(const int *a, const int *b, void *arg),
               void *arg);
```

See ac\_sort\_def, ac\_sort\_m for more details.
