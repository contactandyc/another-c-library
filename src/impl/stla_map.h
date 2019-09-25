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

#define stla_map_find_m(name, keytype, datatype, compare)                      \
  datatype *name(const keytype *p, const stla_map_node_t *root) {              \
    while (root) {                                                             \
      int n = compare(p, (datatype *)root);                                    \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else if (n > 0)                                                          \
        root = root->right;                                                    \
      else                                                                     \
        return (datatype *)root;                                               \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_find2_m(name, keytype, datatype, mapname, compare)            \
  datatype *name(const keytype *p, const stla_map_node_t *root) {              \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d);                                                   \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else if (n > 0)                                                          \
        root = root->right;                                                    \
      else                                                                     \
        return (datatype *)root;                                               \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_find_arg_m(name, keytype, datatype, compare)                  \
  datatype *name(const keytype *p, const stla_map_node_t *root, void *arg) {   \
    while (root) {                                                             \
      int n = compare(p, (datatype *)root, arg);                               \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else if (n > 0)                                                          \
        root = root->right;                                                    \
      else                                                                     \
        return (datatype *)root;                                               \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_find2_arg_m(name, keytype, datatype, mapname, compare)        \
  datatype *name(const keytype *p, const stla_map_node_t *root, void *arg) {   \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d, arg);                                              \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else if (n > 0)                                                          \
        root = root->right;                                                    \
      else                                                                     \
        return (datatype *)root;                                               \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_find_compare_m(name, keytype, datatype)                       \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value)) {     \
    while (root) {                                                             \
      int n = compare(p, (datatype *)root);                                    \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else if (n > 0)                                                          \
        root = root->right;                                                    \
      else                                                                     \
        return (datatype *)root;                                               \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_find2_compare_m(name, keytype, datatype, mapname)             \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value)) {     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d);                                                   \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else if (n > 0)                                                          \
        root = root->right;                                                    \
      else                                                                     \
        return (datatype *)root;                                               \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_find_compare_arg_m(name, keytype, datatype)                   \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value),       \
                 void *arg) {                                                  \
    while (root) {                                                             \
      int n = compare(p, (datatype *)root, arg);                               \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else if (n > 0)                                                          \
        root = root->right;                                                    \
      else                                                                     \
        return (datatype *)root;                                               \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_find2_compare_arg_m(name, keytype, mapname, datatype)         \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value),       \
                 void *arg) {                                                  \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d, arg);                                              \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else if (n > 0)                                                          \
        root = root->right;                                                    \
      else                                                                     \
        return (datatype *)root;                                               \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_least_m(name, keytype, datatype, compare)                     \
  datatype *name(const keytype *p, const stla_map_node_t *root) {              \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d);                                                   \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = parent->parent;                                               \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      if (!compare(p, (datatype *)parent))                                     \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_least2_m(name, keytype, datatype, mapname, compare)           \
  datatype *name(const keytype *p, const stla_map_node_t *root) {              \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d);                                                   \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = parent->parent;                                               \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      if (!compare(p, d))                                                      \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_least_arg_m(name, keytype, datatype, compare)                 \
  datatype *name(const keytype *p, const stla_map_node_t *root, void *arg) {   \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d, arg);                                              \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = stla_map_next(parent);                                        \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      if (!compare(p, (datatype *)parent, arg))                                \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_least2_arg_m(name, keytype, datatype, mapname, compare)       \
  datatype *name(const keytype *p, const stla_map_node_t *root, void *arg) {   \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d, arg);                                              \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = stla_map_next(parent);                                        \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      if (!compare(p, d, arg))                                                 \
        return d;                                                              \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_least_compare_m(name, keytype, datatype)                      \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value)) {     \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d);                                                   \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = parent->parent;                                               \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      if (!compare(p, (datatype *)parent))                                     \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_least2_compare_m(name, keytype, datatype, mapname)            \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value)) {     \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d);                                                   \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = parent->parent;                                               \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      if (!compare(p, d))                                                      \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_least_compare_arg_m(name, keytype, datatype)                  \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value),       \
                 void *arg) {                                                  \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d, arg);                                              \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = stla_map_next(parent);                                        \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      if (!compare(p, (datatype *)parent, arg))                                \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_least2_compare_arg_m(name, keytype, datatype, mapname)        \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value),       \
                 void *arg) {                                                  \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d, arg);                                              \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = stla_map_next(parent);                                        \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      if (!compare(p, d, arg))                                                 \
        return d;                                                              \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_greatest_m(name, keytype, datatype, compare)                  \
  datatype *name(const keytype *p, const stla_map_node_t *root) {              \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d);                                                   \
      if (n > 0)                                                               \
        root = root->right;                                                    \
      else                                                                     \
        root = root->left;                                                     \
    }                                                                          \
    if (parent) {                                                              \
      if (n > 0) {                                                             \
        parent = stla_map_previous(parent);                                    \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      if (!compare(p, (datatype *)parent))                                     \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_greatest2_m(name, keytype, datatype, mapname, compare)        \
  datatype *name(const keytype *p, const stla_map_node_t *root) {              \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d);                                                   \
      if (n > 0)                                                               \
        root = root->right;                                                    \
      else                                                                     \
        root = root->left;                                                     \
    }                                                                          \
    if (parent) {                                                              \
      if (n > 0) {                                                             \
        parent = stla_map_previous(parent);                                    \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      if (!compare(p, d))                                                      \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_greatest_arg_m(name, keytype, datatype, compare)              \
  datatype *name(const keytype *p, const stla_map_node_t *root, void *arg) {   \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d, arg);                                              \
      if (n > 0)                                                               \
        root = root->right;                                                    \
      else                                                                     \
        root = root->left;                                                     \
    }                                                                          \
    if (parent) {                                                              \
      if (n > 0) {                                                             \
        parent = stla_map_previous(parent);                                    \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      if (!compare(p, (datatype *)parent, arg))                                \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_greatest2_arg_m(name, keytype, datatype, mapname, compare)    \
  datatype *name(const keytype *p, const stla_map_node_t *root, void *arg) {   \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d, arg);                                              \
      if (n > 0)                                                               \
        root = root->right;                                                    \
      else                                                                     \
        root = root->left;                                                     \
    }                                                                          \
    if (parent) {                                                              \
      if (n > 0) {                                                             \
        parent = stla_map_previous(parent);                                    \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      if (!compare(p, d, arg))                                                 \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_greatest_compare_m(name, keytype, datatype)                   \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value)) {     \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d);                                                   \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = parent->parent;                                               \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      if (!compare(p, (datatype *)parent))                                     \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_greatest2_compare_m(name, keytype, datatype, mapname)         \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value)) {     \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d);                                                   \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = parent->parent;                                               \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      if (!compare(p, d))                                                      \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_greatest_compare_arg_m(name, keytype, datatype)               \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value),       \
                 void *arg) {                                                  \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d, arg);                                              \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = stla_map_next(parent);                                        \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      if (!compare(p, (datatype *)parent, arg))                                \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_greatest2_compare_arg_m(name, keytype, datatype, mapname)     \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value),       \
                 void *arg) {                                                  \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d, arg);                                              \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = stla_map_next(parent);                                        \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      if (!compare(p, d, arg))                                                 \
        return d;                                                              \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_lower_bound_m(name, keytype, datatype, compare)               \
  datatype *name(const keytype *p, const stla_map_node_t *root) {              \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d);                                                   \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0)                                                               \
        return (datatype *)stla_map_next(parent);                              \
      else                                                                     \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_lower_bound2_m(name, keytype, datatype, mapname, compare)     \
  datatype *name(const keytype *p, const stla_map_node_t *root) {              \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d);                                                   \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = stla_map_next(parent);                                        \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      return stla_parent_object(parent, datatype, mapname);                    \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_lower_bound_arg_m(name, keytype, datatype, compare)           \
  datatype *name(const keytype *p, const stla_map_node_t *root, void *arg) {   \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d, arg);                                              \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0)                                                               \
        return (datatype *)stla_map_next(parent);                              \
      else                                                                     \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_lower_bound2_arg_m(name, keytype, datatype, mapname, compare) \
  datatype *name(const keytype *p, const stla_map_node_t *root, void *arg) {   \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d, arg);                                              \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = stla_map_next(parent);                                        \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      return stla_parent_object(parent, datatype, mapname);                    \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_lower_bound_compare_m(name, keytype, datatype)                \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value)) {     \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d);                                                   \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = parent->parent;                                               \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      if (!compare(p, (datatype *)parent))                                     \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_lower_bound2_compare_m(name, keytype, datatype, mapname)      \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value)) {     \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d);                                                   \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = parent->parent;                                               \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      if (!compare(p, d))                                                      \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_lower_bound_compare_arg_m(name, keytype, datatype)            \
  datatype *name(                                                              \
      const keytype *p, const stla_map_node_t *root,                           \
      int compare(const keytype *key, const datatype *value, void *arg),       \
      void *arg) {                                                             \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d, arg);                                              \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = stla_map_next(parent);                                        \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      if (!compare(p, (datatype *)parent, arg))                                \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_lower_bound2_compare_arg_m(name, keytype, datatype, mapname)  \
  datatype *name(                                                              \
      const keytype *p, const stla_map_node_t *root,                           \
      int compare(const keytype *key, const datatype *value, void *arg),       \
      void *arg) {                                                             \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d, arg);                                              \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = stla_map_next(parent);                                        \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      if (!compare(p, d, arg))                                                 \
        return d;                                                              \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_upper_bound_m(name, keytype, datatype, compare)               \
  datatype *name(const keytype *p, const stla_map_node_t *root) {              \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d);                                                   \
      if (n > 0)                                                               \
        root = root->right;                                                    \
      else                                                                     \
        root = root->left;                                                     \
    }                                                                          \
    if (parent) {                                                              \
      if (n > 0)                                                               \
        return (datatype *)stla_map_previous(parent);                          \
      else                                                                     \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_upper_bound2_m(name, keytype, datatype, mapname, compare)     \
  datatype *name(const keytype *p, const stla_map_node_t *root) {              \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d);                                                   \
      if (n > 0)                                                               \
        root = root->right;                                                    \
      else                                                                     \
        root = root->left;                                                     \
    }                                                                          \
    if (parent) {                                                              \
      if (n > 0) {                                                             \
        parent = stla_map_previous(parent);                                    \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      return stla_parent_object(parent, datatype, mapname);                    \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_upper_bound_arg_m(name, keytype, datatype, compare)           \
  datatype *name(const keytype *p, const stla_map_node_t *root, void *arg) {   \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d, arg);                                              \
      if (n > 0)                                                               \
        root = root->right;                                                    \
      else                                                                     \
        root = root->left;                                                     \
    }                                                                          \
    if (parent) {                                                              \
      if (n > 0)                                                               \
        return (datatype *)stla_map_previous(parent);                          \
      else                                                                     \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_upper_bound2_arg_m(name, keytype, datatype, mapname, compare) \
  datatype *name(const keytype *p, const stla_map_node_t *root, void *arg) {   \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d, arg);                                              \
      if (n > 0)                                                               \
        root = root->right;                                                    \
      else                                                                     \
        root = root->left;                                                     \
    }                                                                          \
    if (parent) {                                                              \
      if (n > 0) {                                                             \
        parent = stla_map_previous(parent);                                    \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      return stla_parent_object(parent, datatype, mapname);                    \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_upper_bound_compare_m(name, keytype, datatype)                \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value)) {     \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d);                                                   \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = parent->parent;                                               \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      if (!compare(p, (datatype *)parent))                                     \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_upper_bound2_compare_m(name, keytype, datatype, mapname)      \
  datatype *name(const keytype *p, const stla_map_node_t *root,                \
                 int compare(const keytype *key, const datatype *value)) {     \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d);                                                   \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = parent->parent;                                               \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      if (!compare(p, d))                                                      \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_upper_bound_compare_arg_m(name, keytype, datatype)            \
  datatype *name(                                                              \
      const keytype *p, const stla_map_node_t *root,                           \
      int compare(const keytype *key, const datatype *value, void *arg),       \
      void *arg) {                                                             \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      parent = root;                                                           \
      int n = compare(p, d, arg);                                              \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = stla_map_next(parent);                                        \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      if (!compare(p, (datatype *)parent, arg))                                \
        return (datatype *)parent;                                             \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_upper_bound2_compare_arg_m(name, keytype, datatype, mapname)  \
  datatype *name(                                                              \
      const keytype *p, const stla_map_node_t *root,                           \
      int compare(const keytype *key, const datatype *value, void *arg),       \
      void *arg) {                                                             \
    stla_map_node_t *parent = NULL;                                            \
    int n;                                                                     \
    while (root) {                                                             \
      parent = root;                                                           \
      datatype *d = stla_parent_object(root, datatype, mapname);               \
      int n = compare(p, d, arg);                                              \
      if (n < 0)                                                               \
        root = root->left;                                                     \
      else                                                                     \
        root = root->right;                                                    \
    }                                                                          \
    if (parent) {                                                              \
      if (n < 0) {                                                             \
        parent = stla_map_next(parent);                                        \
        if (!parent)                                                           \
          return NULL;                                                         \
      }                                                                        \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      if (!compare(p, d, arg))                                                 \
        return d;                                                              \
    }                                                                          \
    return NULL;                                                               \
  }

#define stla_map_insert_m(name, datatype, compare)                             \
  bool name(datatype *node, stla_map_node_t **root) {                          \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      int n = compare(node, (datatype *)parent);                               \
      if (n < 0)                                                               \
        np = &(parent->left);                                                  \
      else if (n > 0)                                                          \
        np = &(parent->right);                                                 \
      else                                                                     \
        return false;                                                          \
    }                                                                          \
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }

#define stla_map_insert2_m(name, datatype, mapname, compare)                   \
  bool name(datatype *node, stla_map_node_t **root) {                          \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      int n = compare(node, d);                                                \
      if (n < 0)                                                               \
        np = &(parent->left);                                                  \
      else if (n > 0)                                                          \
        np = &(parent->right);                                                 \
      else                                                                     \
        return false;                                                          \
    }                                                                          \
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }

#define stla_map_insert_arg_m(name, datatype, compare)                         \
  bool name(datatype *node, stla_map_node_t **root, void *arg) {               \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      int n = compare(node, (datatype *)parent, arg);                          \
      if (n < 0)                                                               \
        np = &(parent->left);                                                  \
      else if (n > 0)                                                          \
        np = &(parent->right);                                                 \
      else                                                                     \
        return false;                                                          \
    }                                                                          \
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }

#define stla_map_insert2_arg_m(name, datatype, mapname, compare)               \
  bool name(datatype *node, stla_map_node_t **root, void *arg) {               \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      int n = compare(node, d, arg);                                           \
      if (n < 0)                                                               \
        np = &(parent->left);                                                  \
      else if (n > 0)                                                          \
        np = &(parent->right);                                                 \
      else                                                                     \
        return false;                                                          \
    }                                                                          \
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }

#define stla_map_insert_compare_m(name, datatype)                              \
  bool name(datatype *node, stla_map_node_t **root,                            \
            int compare(const datatype *key, const datatype *value)) {         \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      int n = compare(node, (datatype *)parent);                               \
      if (n < 0)                                                               \
        np = &(parent->left);                                                  \
      else if (n > 0)                                                          \
        np = &(parent->right);                                                 \
      else                                                                     \
        return false;                                                          \
    }                                                                          \
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }

#define stla_map_insert2_compare_m(name, datatype)                             \
  bool name(datatype *node, stla_map_node_t **root,                            \
            int compare(const datatype *key, const datatype *value)) {         \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      int n = compare(node, d);                                                \
      if (n < 0)                                                               \
        np = &(parent->left);                                                  \
      else if (n > 0)                                                          \
        np = &(parent->right);                                                 \
      else                                                                     \
        return false;                                                          \
    }                                                                          \
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }

#define stla_map_insert_compare_arg_m(name, datatype)                          \
  bool name(                                                                   \
      datatype *node, stla_map_node_t **root,                                  \
      int compare(const datatype *key, const datatype *value, void *arg),      \
      void *arg) {                                                             \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      int n = compare(node, (datatype *)parent, arg);                          \
      if (n < 0)                                                               \
        np = &(parent->left);                                                  \
      else if (n > 0)                                                          \
        np = &(parent->right);                                                 \
      else                                                                     \
        return false;                                                          \
    }                                                                          \
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }

#define stla_map_insert2_compare_arg_m(name, datatype)                         \
  bool name(                                                                   \
      datatype *node, stla_map_node_t **root,                                  \
      int compare(const datatype *key, const datatype *value, void *arg),      \
      void *arg) {                                                             \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      int n = compare(node, d, arg);                                           \
      if (n < 0)                                                               \
        np = &(parent->left);                                                  \
      else if (n > 0)                                                          \
        np = &(parent->right);                                                 \
      else                                                                     \
        return false;                                                          \
    }                                                                          \
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }

#define stla_multimap_insert_m(name, datatype, compare)                        \
  bool name(datatype *node, stla_map_node_t **root) {                          \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      int n = compare(node, (datatype *)parent);                               \
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
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }

#define stla_multimap_insert2_m(name, datatype, mapname, compare)              \
  bool name(datatype *node, stla_map_node_t **root) {                          \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      int n = compare(node, d);                                                \
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
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }

#define stla_multimap_insert_arg_m(name, datatype, compare)                    \
  bool name(datatype *node, stla_map_node_t **root, void *arg) {               \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      int n = compare(node, (datatype *)parent, arg);                          \
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
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }

#define stla_multimap_insert2_arg_m(name, datatype, mapname, compare)          \
  bool name(datatype *node, stla_map_node_t **root, void *arg) {               \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      int n = compare(node, d, arg);                                           \
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
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }

#define stla_multimap_insert_compare_m(name, datatype)                         \
  bool name(datatype *node, stla_map_node_t **root,                            \
            int compare(const datatype *key, const datatype *value)) {         \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      int n = compare(node, (datatype *)parent);                               \
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
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }

#define stla_multimap_insert2_compare_m(name, datatype, mapname)               \
  bool name(datatype *node, stla_map_node_t **root,                            \
            int compare(const datatype *key, const datatype *value)) {         \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      int n = compare(node, d);                                                \
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
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }

#define stla_multimap_insert_compare_arg_m(name, datatype)                     \
  bool name(                                                                   \
      datatype *node, stla_map_node_t **root,                                  \
      int compare(const datatype *key, const datatype *value, void *arg),      \
      void *arg) {                                                             \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      int n = compare(node, (datatype *)parent, arg);                          \
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
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }

#define stla_multimap_insert2_compare_arg_m(name, datatype, mapname)           \
  bool name(                                                                   \
      datatype *node, stla_map_node_t **root,                                  \
      int compare(const datatype *key, const datatype *value, void *arg),      \
      void *arg) {                                                             \
    stla_map_node_t **np = root, *parent = NULL;                               \
    while (*np) {                                                              \
      parent = *np;                                                            \
      datatype *d = stla_parent_object(parent, datatype, mapname);             \
      int n = compare(node, d, arg);                                           \
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
    *np = (stla_map_node_t *)node;                                             \
    stla_map_fix_insert(*np, parent, root);                                    \
    return true;                                                               \
  }
