/*
Copyright 2019 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#define ac_search_compare_arg_def(name, keytype, datatype)                     \
  datatype *name(keytype *p, datatype *base, size_t num_elements,              \
                 int compare(keytype *key, datatype *value, void *arg),        \
                 void *arg);

#define ac_search_compare_arg_m(name, keytype, datatype, compare)              \
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
        return mid;                                                            \
    }                                                                          \
    return NULL;                                                               \
  }

#define ac_search_least_compare_arg_m(name, keytype, datatype, compare)        \
  datatype *name(keytype *p, datatype *base, size_t num_elements,              \
                 int compare(keytype *key, datatype *value, void *arg),        \
                 void *arg) {                                                  \
    if (!num_elements)                                                         \
      return NULL;                                                             \
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

#define ac_search_greatest_compare_arg_m(name, keytype, datatype, compare)     \
  datatype *name(keytype *p, datatype *base, size_t num_elements,              \
                 int compare(keytype *key, datatype *value, void *arg),        \
                 void *arg) {                                                  \
    if (!num_elements)                                                         \
      return NULL;                                                             \
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

#define ac_search_lower_bound_compare_arg_m(name, keytype, datatype, compare)  \
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

#define ac_search_upper_bound_compare_arg_m(name, keytype, datatype, compare)  \
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
