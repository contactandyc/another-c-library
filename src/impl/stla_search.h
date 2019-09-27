#include "stla_common.h"

#include <stdio.h>

#define stla_search_m(name, keytype, datatype, compare)                        \
  datatype *name(keytype *p, datatype *base, size_t num_elements) {            \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid);                                                 \
      if (n > 0)                                                               \
        low = mid + 1;                                                         \
      else if (n < 0)                                                          \
        high = mid;                                                            \
      else                                                                     \
        return low;                                                            \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_search_least_m(name, keytype, datatype, compare)                  \
  datatype *name(keytype *p, datatype *base, size_t num_elements) {            \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid);                                                 \
      if (n > 0)                                                               \
        low = mid + 1;                                                         \
      else                                                                     \
        high = mid;                                                            \
    }                                                                          \
    if (!compare(p, low))                                                      \
      return low;                                                              \
    return NULL;                                                               \
  }

#define stla_search_greatest_m(name, keytype, datatype, compare)               \
  datatype **name(keytype *p, datatype *base, size_t num_elements) {           \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    datatype *res = low;                                                       \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid);                                                 \
      if (n < 0)                                                               \
        high = mid;                                                            \
      else {                                                                   \
        res = mid;                                                             \
        low = mid + 1;                                                         \
      }                                                                        \
    }                                                                          \
    if (!compare(p, res))                                                      \
      return res;                                                              \
    return NULL;                                                               \
  }

#define stla_search_lower_bound_m(name, keytype, datatype, compare)            \
  datatype *name(char *p, datatype *base, size_t num_elements) {               \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid);                                                 \
      if (n > 0)                                                               \
        low = mid + 1;                                                         \
      else                                                                     \
        high = mid;                                                            \
    }                                                                          \
    return low;                                                                \
  }

#define stla_search_upper_bound_m(name, keytype, datatype, compare)            \
  datatype *name(char *p, datatype *base, size_t num_elements) {               \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid);                                                 \
      if (n < 0)                                                               \
        high = mid;                                                            \
      else                                                                     \
        low = mid + 1;                                                         \
    }                                                                          \
    return low;                                                                \
  }

#define stla_search_compare_m(name, keytype, datatype)                         \
  datatype *name(keytype *p, datatype *base, size_t num_elements,              \
                 int compare(keytype *key, datatype *value)) {                 \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid);                                                 \
      if (n > 0)                                                               \
        low = mid + 1;                                                         \
      else if (n < 0)                                                          \
        high = mid;                                                            \
      else                                                                     \
        return low;                                                            \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_search_least_compare_m(name, keytype, datatype)                   \
  datatype *name(keytype *p, datatype *base, size_t num_elements,              \
                 int compare(keytype *key, datatype *value)) {                 \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid);                                                 \
      if (n > 0)                                                               \
        low = mid + 1;                                                         \
      else                                                                     \
        high = mid;                                                            \
    }                                                                          \
    if (!compare(p, low))                                                      \
      return low;                                                              \
    return NULL;                                                               \
  }

#define stla_search_greatest_compare_m(name, keytype, datatype)                \
  datatype *name(keytype *p, datatype *base, size_t num_elements,              \
                 int compare(keytype *key, datatype *value)) {                 \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    datatype *res = low;                                                       \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid);                                                 \
      if (n < 0)                                                               \
        high = mid;                                                            \
      else {                                                                   \
        res = mid;                                                             \
        low = mid + 1;                                                         \
      }                                                                        \
    }                                                                          \
    if (!compare(p, res))                                                      \
      return res;                                                              \
    return NULL;                                                               \
  }

#define stla_search_lower_bound_compare_m(name, keytype, datatype)             \
  datatype *name(keytype *p, datatype *base, size_t num_elements,              \
                 int compare(keytype *key, datatype *value)) {                 \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid);                                                 \
      if (n > 0)                                                               \
        low = mid + 1;                                                         \
      else                                                                     \
        high = mid;                                                            \
    }                                                                          \
    return low;                                                                \
  }

#define stla_search_upper_bound_compare_m(name, keytype, datatype)             \
  datatype *name(keytype *p, datatype *base, size_t num_elements,              \
                 int compare(keytype *key, datatype *value)) {                 \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid);                                                 \
      if (n < 0)                                                               \
        high = mid;                                                            \
      else                                                                     \
        low = mid + 1;                                                         \
    }                                                                          \
    return low;                                                                \
  }

#define stla_search_arg_m(name, keytype, datatype, compare)                    \
  datatype *name(keytype *p, datatype *base, size_t num_elements, void *arg) { \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid, arg);                                            \
      if (n > 0)                                                               \
        low = mid + 1;                                                         \
      else if (n < 0)                                                          \
        high = mid;                                                            \
      else                                                                     \
        return low;                                                            \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_search_least_arg_m(name, keytype, datatype, compare)              \
  datatype *name(keytype *p, datatype *base, size_t num_elements, void *arg) { \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid, arg);                                            \
      if (n > 0)                                                               \
        low = mid + 1;                                                         \
      else                                                                     \
        high = mid;                                                            \
    }                                                                          \
    if (!compare(p, low, arg))                                                 \
      return low;                                                              \
    return NULL;                                                               \
  }

#define stla_search_greatest_arg_m(name, keytype, datatype, compare)           \
  datatype **name(keytype *p, datatype *base, size_t num_elements,             \
                  void *arg) {                                                 \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    datatype *res = low;                                                       \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid, arg);                                            \
      if (n < 0)                                                               \
        high = mid;                                                            \
      else {                                                                   \
        res = mid;                                                             \
        low = mid + 1;                                                         \
      }                                                                        \
    }                                                                          \
    if (!compare(p, res, arg))                                                 \
      return res;                                                              \
    return NULL;                                                               \
  }

#define stla_search_lower_bound_arg_m(name, keytype, datatype, compare)        \
  datatype *name(char *p, datatype *base, size_t num_elements, void *arg) {    \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid, arg);                                            \
      if (n > 0)                                                               \
        low = mid + 1;                                                         \
      else                                                                     \
        high = mid;                                                            \
    }                                                                          \
    return low;                                                                \
  }

#define stla_search_upper_bound_arg_m(name, keytype, datatype, compare)        \
  datatype *name(char *p, datatype *base, size_t num_elements, void *arg) {    \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid, arg);                                            \
      if (n < 0)                                                               \
        high = mid;                                                            \
      else                                                                     \
        low = mid + 1;                                                         \
    }                                                                          \
    return low;                                                                \
  }

#define stla_search_compare_arg_m(name, keytype, datatype, compare)            \
  datatype *name(keytype *p, datatype *base, size_t num_elements,              \
                 int compare(keytype *key, datatype *value, void *arg),        \
                 void *arg) {                                                  \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid, arg);                                            \
      if (n > 0)                                                               \
        low = mid + 1;                                                         \
      else if (n < 0)                                                          \
        high = mid;                                                            \
      else                                                                     \
        return low;                                                            \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_search_least_compare_arg_m(name, keytype, datatype, compare)      \
  datatype *name(keytype *p, datatype *base, size_t num_elements,              \
                 int compare(keytype *key, datatype *value, void *arg),        \
                 void *arg) {                                                  \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid, arg);                                            \
      if (n > 0)                                                               \
        low = mid + 1;                                                         \
      else                                                                     \
        high = mid;                                                            \
    }                                                                          \
    if (!compare(p, low, arg))                                                 \
      return low;                                                              \
    return NULL;                                                               \
  }

#define stla_search_greatest_compare_arg_m(name, keytype, datatype, compare)   \
  datatype *name(keytype *p, datatype *base, size_t num_elements,              \
                 int compare(keytype *key, datatype *value, void *arg),        \
                 void *arg) {                                                  \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    datatype *res = low;                                                       \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid, arg);                                            \
      if (n < 0)                                                               \
        high = mid;                                                            \
      else {                                                                   \
        res = mid;                                                             \
        low = mid + 1;                                                         \
      }                                                                        \
    }                                                                          \
    if (!compare(p, res, arg))                                                 \
      return res;                                                              \
    return NULL;                                                               \
  }

#define stla_search_lower_bound_compare_arg_m(name, keytype, datatype,         \
                                              compare)                         \
  datatype *name(keytype *p, datatype *base, size_t num_elements,              \
                 int compare(keytype *key, datatype *value, void *arg),        \
                 void *arg) {                                                  \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid, arg);                                            \
      if (n > 0)                                                               \
        low = mid + 1;                                                         \
      else                                                                     \
        high = mid;                                                            \
    }                                                                          \
    return low;                                                                \
  }

#define stla_search_upper_bound_compare_arg_m(name, keytype, datatype,         \
                                              compare)                         \
  datatype *name(keytype *p, datatype *base, size_t num_elements,              \
                 int compare(keytype *key, datatype *value, void *arg),        \
                 void *arg) {                                                  \
    datatype *low = base;                                                      \
    datatype *high = base + num_elements;                                      \
    datatype *mid;                                                             \
    while (low < high) {                                                       \
      mid = low + ((high - low) >> 1);                                         \
      int n = compare(p, mid, arg);                                            \
      if (n < 0)                                                               \
        high = mid;                                                            \
      else                                                                     \
        low = mid + 1;                                                         \
    }                                                                          \
    return low;                                                                \
  }
