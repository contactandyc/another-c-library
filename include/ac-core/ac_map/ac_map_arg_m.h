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

#define ac_map_find_arg_def(name, keytype, datatype)                           \
  datatype *name(const keytype *p, const ac_map_t *root, void *arg);

#define ac_map_insert_arg_def(name, datatype, compare)                         \
  bool name(datatype *node, ac_map_t **root, void *arg);

#define ac_map_find_arg_m(name, keytype, datatype, compare)                    \
  datatype *name(const keytype *p, const ac_map_t *root, void *arg) {          \
    while (root) {                                                             \
      int n = compare((const keytype *)p, (const datatype *)root, arg);        \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else if (n > 0)                                                          \
        root = root->right;                                                    \
      else                                                                     \
        return (datatype *)root;                                               \
    }                                                                          \
    return NULL;                                                               \
  }

#define ac_map_least_arg_m(name, keytype, datatype, compare)                   \
  datatype *name(const keytype *p, const ac_map_t *root, void *arg) {          \
    int n = 0;                                                                 \
    datatype *res = NULL;                                                      \
    while (root) {                                                             \
      n = compare((const keytype *)p, (const datatype *)root, arg);            \
      if (n <= 0) {                                                            \
        res = (datatype *)root;                                                \
        root = root->left;                                                     \
      } else                                                                   \
        root = root->right;                                                    \
    }                                                                          \
    if (res && !compare((const keytype *)p, (const datatype *)res), arg)       \
      return res;                                                              \
    return NULL;                                                               \
  }

#define ac_map_greatest_arg_m(name, keytype, datatype, compare)                \
  datatype *name(const keytype *p, const ac_map_t *root, void *arg) {          \
    int n = 0;                                                                 \
    datatype *res = NULL;                                                      \
    while (root) {                                                             \
      n = compare((const keytype *)p, (const datatype *)root, arg);            \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else {                                                                   \
        res = (datatype *)root;                                                \
        root = root->right;                                                    \
      }                                                                        \
    }                                                                          \
    if (res && !compare((const keytype *)p, (const datatype *)res, arg))       \
      return res;                                                              \
    return NULL;                                                               \
  }

#define ac_map_lower_bound_arg_m(name, keytype, datatype, compare)             \
  datatype *name(const keytype *p, const ac_map_t *root, void *arg) {          \
    int n = 0;                                                                 \
    datatype *res = NULL;                                                      \
    while (root) {                                                             \
      n = compare((const keytype *)p, (const datatype *)root, arg);            \
      if (n <= 0) {                                                            \
        res = (datatype *)root;                                                \
        root = root->left;                                                     \
      } else                                                                   \
        root = root->right;                                                    \
    }                                                                          \
    return res;                                                                \
  }

#define ac_map_upper_bound_arg_m(name, keytype, datatype, compare)             \
  datatype *name(const keytype *p, const ac_map_t *root, void *arg) {          \
    int n = 0;                                                                 \
    datatype *res = NULL;                                                      \
    while (root) {                                                             \
      n = compare((const keytype *)p, (const datatype *)root, arg);            \
      if (n < 0) {                                                             \
        res = (datatype *)root;                                                \
        root = root->left;                                                     \
      } else                                                                   \
        root = root->right;                                                    \
    }                                                                          \
    return res;                                                                \
  }

#define ac_map_insert_arg_m(name, datatype, compare)                           \
  bool name(datatype *node, ac_map_t **root, void *arg) {                      \
    ac_map_t **np = root, *parent = NULL;                                      \
    while (*np) {                                                              \
      parent = *np;                                                            \
      int n = compare((const datatype *)node, (const datatype *)parent, arg);  \
      if (n < 0)                                                               \
        np = &(parent->left);                                                  \
      else if (n > 0)                                                          \
        np = &(parent->right);                                                 \
      else                                                                     \
        return false;                                                          \
    }                                                                          \
    *np = (ac_map_t *)node;                                                    \
    ac_map_fix_insert(*np, parent, root);                                      \
    return true;                                                               \
  }

#define ac_multimap_insert_arg_m(name, datatype, compare)                      \
  bool name(datatype *node, ac_map_t **root, void *arg) {                      \
    ac_map_t **np = root, *parent = NULL;                                      \
    while (*np) {                                                              \
      parent = *np;                                                            \
      int n = compare((const datatype *)node, (const datatype *)parent, arg);  \
      if (n < 0)                                                               \
        np = &(parent->left);                                                  \
      else if (n > 0)                                                          \
        np = &(parent->right);                                                 \
      else {                                                                   \
        if (node < (datatype *)parent)                                         \
          np = &(parent->left);                                                \
        else if (node > (datatype *)parent)                                    \
          np = &(parent->right);                                               \
        else                                                                   \
          return false;                                                        \
      }                                                                        \
    }                                                                          \
    *np = (ac_map_t *)node;                                                    \
    ac_map_fix_insert(*np, parent, root);                                      \
    return true;                                                               \
  }
