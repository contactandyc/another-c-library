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

#define ac_map_find2_compare_m(name, keytype, datatype, mapname)               \
  datatype *name(const keytype *p, const ac_map_t *root,                       \
                 int compare(const keytype *key, const datatype *value)) {     \
    while (root) {                                                             \
      datatype *d = ac_parent_object(root, datatype, mapname);                 \
      int n = compare((const keytype *)p, (const datatype *)d);                \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else if (n > 0)                                                          \
        root = root->right;                                                    \
      else                                                                     \
        return (datatype *)d;                                                  \
    }                                                                          \
    return NULL;                                                               \
  }

#define ac_map_least2_compare_m(name, keytype, datatype, mapname)              \
  datatype *name(const keytype *p, const ac_map_t *root,                       \
                 int compare(const keytype *key, const datatype *value)) {     \
    int n = 0;                                                                 \
    datatype *res = NULL;                                                      \
    while (root) {                                                             \
      datatype *d = ac_parent_object(root, datatype, mapname);                 \
      n = compare((const keytype *)p, (const datatype *)d);                    \
      if (n <= 0) {                                                            \
        res = d;                                                               \
        root = root->left;                                                     \
      } else                                                                   \
        root = root->right;                                                    \
    }                                                                          \
    if (res && !compare((const keytype *)p, (const datatype *)res))            \
      return res;                                                              \
    return NULL;                                                               \
  }

#define ac_map_greatest2_compare_m(name, keytype, datatype, mapname)           \
  datatype *name(const keytype *p, const ac_map_t *root,                       \
                 int compare(const keytype *key, const datatype *value)) {     \
    int n = 0;                                                                 \
    datatype *res = NULL;                                                      \
    while (root) {                                                             \
      datatype *d = ac_parent_object(root, datatype, mapname);                 \
      n = compare((const keytype *)p, (const datatype *)d);                    \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else {                                                                   \
        res = d;                                                               \
        root = root->right;                                                    \
      }                                                                        \
    }                                                                          \
    if (res && !compare((const keytype *)p, (const datatype *)res))            \
      return res;                                                              \
    return NULL;                                                               \
  }

#define ac_map_lower_bound2_compare_m(name, keytype, datatype, mapname)        \
  datatype *name(const keytype *p, const ac_map_t *root,                       \
                 int compare(const keytype *key, const datatype *value)) {     \
    int n = 0;                                                                 \
    datatype *res = NULL;                                                      \
    while (root) {                                                             \
      datatype *d = ac_parent_object(root, datatype, mapname);                 \
      n = compare((const keytype *)p, (const datatype *)d);                    \
      if (n <= 0) {                                                            \
        res = d;                                                               \
        root = root->left;                                                     \
      } else                                                                   \
        root = root->right;                                                    \
    }                                                                          \
    return res;                                                                \
  }

#define ac_map_upper_bound2_compare_m(name, keytype, datatype, mapname)        \
  datatype *name(const keytype *p, const ac_map_t *root,                       \
                 int compare(const keytype *key, const datatype *value)) {     \
    int n = 0;                                                                 \
    datatype *res = NULL;                                                      \
    while (root) {                                                             \
      datatype *d = ac_parent_object(root, datatype, mapname);                 \
      n = compare((const keytype *)p, (const datatype *)d);                    \
      if (n < 0) {                                                             \
        res = d;                                                               \
        root = root->left;                                                     \
      } else                                                                   \
        root = root->right;                                                    \
    }                                                                          \
    return res;                                                                \
  }

#define ac_map_insert2_compare_m(name, datatype, mapname)                      \
  bool name(datatype *node, ac_map_t **root,                                   \
            int compare(const datatype *key, const datatype *value)) {         \
    ac_map_t **np = root, *parent = NULL;                                      \
    while (*np) {                                                              \
      parent = *np;                                                            \
      datatype *d = ac_parent_object(parent, datatype, mapname);               \
      int n = compare((const datatype *)node, (const datatype *)d);            \
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

#define ac_multimap_insert2_compare_m(name, datatype, mapname)                 \
  bool name(datatype *node, ac_map_t **root,                                   \
            int compare(const datatype *key, const datatype *value)) {         \
    ac_map_t **np = root, *parent = NULL;                                      \
    while (*np) {                                                              \
      parent = *np;                                                            \
      datatype *d = ac_parent_object(parent, datatype, mapname);               \
      int n = compare((const datatype *)node, (const datatype *)d);            \
      if (n < 0)                                                               \
        np = &(parent->left);                                                  \
      else if (n > 0)                                                          \
        np = &(parent->right);                                                 \
      else {                                                                   \
        if (node < d)                                                          \
          np = &(parent->left);                                                \
        else if (node > d)                                                     \
          np = &(parent->right);                                               \
        else                                                                   \
          return false;                                                        \
      }                                                                        \
    }                                                                          \
    *np = (ac_map_t *)node;                                                    \
    ac_map_fix_insert(*np, parent, root);                                      \
    return true;                                                               \
  }
