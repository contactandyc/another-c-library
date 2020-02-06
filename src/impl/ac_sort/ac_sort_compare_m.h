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

#define ac_sort_compare_def(name, type)                                        \
  void name(type *base, size_t num_elements,                                   \
            int (*compare)(const type *a, const type *b));

#define ac_sort_compare_m(name, type)                                          \
  void name(type *base, size_t num_elements,                                   \
            int (*compare)(const type *a, const type *b)) {                    \
    type *a;                                                                   \
    type *b;                                                                   \
    type *c;                                                                   \
    type *d;                                                                   \
    type *e;                                                                   \
    type *mid;                                                                 \
    type *high;                                                                \
    type tmp;                                                                  \
    ssize_t pivot;                                                             \
    sort_stack_t stack[64];                                                    \
    sort_stack_t *top = stack;                                                 \
    top->base = NULL;                                                          \
    top++;                                                                     \
    if (num_elements > 12) {                                                   \
      a = base;                                                                \
      high = e = base + num_elements - 1;                                      \
      pivot = num_elements >> 1;                                               \
      c = a + pivot;                                                           \
      if (compare((const type *)e, (const type *)a) < 0) {                     \
        b = a;                                                                 \
        a = e;                                                                 \
        e = b;                                                                 \
        pivot >>= 1;                                                           \
        b = c + pivot;                                                         \
        d = e + pivot;                                                         \
        pivot = -1;                                                            \
      } else {                                                                 \
        pivot >>= 1;                                                           \
        b = a + pivot;                                                         \
        d = c + pivot;                                                         \
        pivot = 1;                                                             \
      }                                                                        \
                                                                               \
      if (compare((const type *)d, (const type *)c) < 0) {                     \
        pivot = 0;                                                             \
        if (compare((const type *)e, (const type *)d) < 0) {                   \
          sort_swap(c, e);                                                     \
        } else {                                                               \
          sort_swap(c, d);                                                     \
        }                                                                      \
      } else if (compare((const type *)e, (const type *)c) < 0) {              \
        pivot = 0;                                                             \
        sort_swap(c, e);                                                       \
      }                                                                        \
      if (compare((const type *)c, (const type *)a) < 0) {                     \
        pivot = 0;                                                             \
        sort_swap(a, c);                                                       \
        if (compare((const type *)d, (const type *)c) < 0) {                   \
          if (compare((const type *)e, (const type *)d) < 0) {                 \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (compare((const type *)e, (const type *)c) < 0) {            \
          sort_swap(c, e);                                                     \
        }                                                                      \
      }                                                                        \
      if (compare((const type *)c, (const type *)b) < 0) {                     \
        pivot = 0;                                                             \
        sort_swap(b, c);                                                       \
        if (compare((const type *)d, (const type *)c) < 0) {                   \
          if (compare((const type *)e, (const type *)d) < 0) {                 \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (compare((const type *)e, (const type *)c) < 0) {            \
          sort_swap(c, e);                                                     \
        }                                                                      \
      }                                                                        \
                                                                               \
      if (!pivot || compare((const type *)b, (const type *)a) < 0 ||           \
          compare((const type *)c, (const type *)b) < 0 ||                     \
          compare((const type *)d, (const type *)c) < 0 ||                     \
          compare((const type *)e, (const type *)d) < 0) {                     \
        if (e < a) {                                                           \
          mid = a;                                                             \
          a = e;                                                               \
          e = mid;                                                             \
          mid = b;                                                             \
          b = d;                                                               \
          d = mid;                                                             \
          sort_swap(a, e);                                                     \
          sort_swap(b, d);                                                     \
        }                                                                      \
        sort_swap(b, a + 1);                                                   \
        sort_swap(d, e - 1);                                                   \
        b = a + 2;                                                             \
        d = e - 2;                                                             \
      } else {                                                                 \
        if (pivot == 1) {                                                      \
          mid = a + 1;                                                         \
          while (a < e) {                                                      \
            if (compare((const type *)mid, (const type *)a) < 0)               \
              break;                                                           \
            a++;                                                               \
            mid++;                                                             \
          }                                                                    \
          if (a >= e)                                                          \
            return;                                                            \
          if (a - base < 4) {                                                  \
            a = base;                                                          \
            sort_swap(b, a + 1);                                               \
            sort_swap(d, e - 1);                                               \
            b = a + 2;                                                         \
            d = e - 2;                                                         \
          } else {                                                             \
            a -= 2;                                                            \
            if (a < c) {                                                       \
              sort_swap(d, e - 1);                                             \
              d = e - 2;                                                       \
              if (a < b) {                                                     \
                if (compare((const type *)b, (const type *)a) < 0) {           \
                  a = base;                                                    \
                  sort_swap(b, a + 1);                                         \
                  b = a + 2;                                                   \
                } else {                                                       \
                  b = a;                                                       \
                  a = base;                                                    \
                }                                                              \
              } else {                                                         \
                if (compare((const type *)c, (const type *)a) < 0) {           \
                  a = base;                                                    \
                } else {                                                       \
                  b = a;                                                       \
                  a = base;                                                    \
                }                                                              \
              }                                                                \
            } else {                                                           \
              if (a < d) {                                                     \
                if (compare((const type *)d, (const type *)a) < 0) {           \
                  b = c;                                                       \
                  a = base;                                                    \
                  sort_swap(d, e - 1);                                         \
                  d = e - 2;                                                   \
                } else {                                                       \
                  b = a;                                                       \
                  c = d;                                                       \
                  a = base;                                                    \
                  d = e - 1;                                                   \
                }                                                              \
              } else {                                                         \
                if (compare((const type *)e, (const type *)a) < 0) {           \
                  b = d;                                                       \
                  c = d;                                                       \
                } else {                                                       \
                  b = a;                                                       \
                  c = a;                                                       \
                }                                                              \
                a = base;                                                      \
                d = e - 1;                                                     \
              }                                                                \
            }                                                                  \
          }                                                                    \
        } else {                                                               \
          if (e < a) {                                                         \
            mid = a;                                                           \
            a = e;                                                             \
            e = mid;                                                           \
            mid = b;                                                           \
            b = d;                                                             \
            d = mid;                                                           \
          }                                                                    \
          mid = e - 1;                                                         \
          high = e;                                                            \
          while (mid > a) {                                                    \
            if (compare((const type *)mid, (const type *)high) < 0)            \
              break;                                                           \
            high--;                                                            \
            mid--;                                                             \
          }                                                                    \
          if (mid <= a) {                                                      \
            while (a < e) {                                                    \
              sort_swap(a, e);                                                 \
              a++;                                                             \
              e--;                                                             \
            }                                                                  \
            return;                                                            \
          } else {                                                             \
            sort_swap(a, e);                                                   \
            sort_swap(b, d);                                                   \
            sort_swap(b, a + 1);                                               \
            sort_swap(d, e - 1);                                               \
            b = a + 2;                                                         \
            d = e - 2;                                                         \
            high = e;                                                          \
          }                                                                    \
        }                                                                      \
      }                                                                        \
                                                                               \
      e = c;                                                                   \
      while (1) {                                                              \
        while (compare((const type *)b, (const type *)c) < 0)                  \
          b++;                                                                 \
        while (compare((const type *)c, (const type *)d) < 0)                  \
          d--;                                                                 \
        if (b < d) {                                                           \
          sort_swap(b, d);                                                     \
          if (b == c)                                                          \
            c = d;                                                             \
          else if (d == c)                                                     \
            c = b;                                                             \
          b++;                                                                 \
          d--;                                                                 \
        } else                                                                 \
          break;                                                               \
      }                                                                        \
      if (e != c) {                                                            \
        if (c <= d) {                                                          \
          sort_swap(c, d);                                                     \
          pivot = d - a;                                                       \
        } else {                                                               \
          sort_swap(c, b);                                                     \
          pivot = b - a;                                                       \
        }                                                                      \
      } else {                                                                 \
        pivot = c - a;                                                         \
      }                                                                        \
      if ((pivot << 1) < num_elements) {                                       \
        if (pivot == 0 &&                                                      \
            !(compare((const type *)base, (const type *)base + 1))) {          \
          a = base + 2;                                                        \
          while (a <= high && !(compare((const type *)base, (const type *)a))) \
            a++;                                                               \
          num_elements -= (a - base);                                          \
          base = a;                                                            \
        } else {                                                               \
          top->base = base + pivot + 1;                                        \
          top->num_elements = num_elements - (pivot + 1);                      \
          num_elements = pivot;                                                \
          top++;                                                               \
        }                                                                      \
      } else {                                                                 \
        top->base = base;                                                      \
        top->num_elements = pivot;                                             \
        base += pivot + 1;                                                     \
        num_elements -= (pivot + 1);                                           \
        top++;                                                                 \
      }                                                                        \
    }                                                                          \
                                                                               \
    while (1) {                                                                \
      if (num_elements < 40) {                                                 \
        if (num_elements < 6) {                                                \
          if (num_elements == 2) {                                             \
            if (compare((const type *)base + 1, (const type *)base) < 0) {     \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
          } else if (num_elements == 3) {                                      \
            if (compare((const type *)base + 1, (const type *)base) < 0) {     \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
            if (compare((const type *)base + 2, (const type *)base + 1) < 0) { \
              sort_swap(base + 1, base + 2);                                   \
              if (compare((const type *)base + 1, (const type *)base) < 0) {   \
                sort_swap(base, base + 1);                                     \
              }                                                                \
            }                                                                  \
          } else if (num_elements == 4) {                                      \
            if (compare((const type *)base + 1, (const type *)base) < 0) {     \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
            if (compare((const type *)base + 3, (const type *)base + 2) < 0) { \
              sort_swap(base + 2, base + 3);                                   \
            }                                                                  \
            if (compare((const type *)base + 2, (const type *)base) < 0) {     \
              if (compare((const type *)base + 3, (const type *)base) < 0) {   \
                sort_swap(base, base + 2);                                     \
                sort_swap(base + 1, base + 3);                                 \
              } else {                                                         \
                sort_swap(base, base + 1);                                     \
                sort_swap(base + 0, base + 2);                                 \
                if (compare((const type *)base + 3, (const type *)base + 2) <  \
                    0) {                                                       \
                  sort_swap(base + 2, base + 3);                               \
                }                                                              \
              }                                                                \
            } else {                                                           \
              if (compare((const type *)base + 2, (const type *)base + 1) <    \
                  0) {                                                         \
                sort_swap(base + 1, base + 2);                                 \
                if (compare((const type *)base + 3, (const type *)base + 2) <  \
                    0) {                                                       \
                  sort_swap(base + 2, base + 3);                               \
                }                                                              \
              }                                                                \
            }                                                                  \
          } else if (num_elements == 5) {                                      \
            a = base;                                                          \
            b = a + 1;                                                         \
            c = b + 1;                                                         \
            d = c + 1;                                                         \
            e = d + 1;                                                         \
                                                                               \
            if (compare((const type *)d, (const type *)c) < 0) {               \
              pivot = 0;                                                       \
              if (compare((const type *)e, (const type *)d) < 0) {             \
                sort_swap(c, e);                                               \
              } else {                                                         \
                sort_swap(c, d);                                               \
              }                                                                \
            } else if (compare((const type *)e, (const type *)c) < 0) {        \
              pivot = 0;                                                       \
              sort_swap(c, e);                                                 \
            }                                                                  \
            if (compare((const type *)c, (const type *)a) < 0) {               \
              pivot = 0;                                                       \
              sort_swap(a, c);                                                 \
              if (compare((const type *)d, (const type *)c) < 0) {             \
                if (compare((const type *)e, (const type *)d) < 0) {           \
                  sort_swap(c, e);                                             \
                } else {                                                       \
                  sort_swap(c, d);                                             \
                }                                                              \
              } else if (compare((const type *)e, (const type *)c) < 0) {      \
                sort_swap(c, e);                                               \
              }                                                                \
            }                                                                  \
            if (compare((const type *)c, (const type *)b) < 0) {               \
              pivot = 0;                                                       \
              sort_swap(b, c);                                                 \
              if (compare((const type *)d, (const type *)c) < 0) {             \
                if (compare((const type *)e, (const type *)d) < 0) {           \
                  sort_swap(c, e);                                             \
                } else {                                                       \
                  sort_swap(c, d);                                             \
                }                                                              \
              } else if (compare((const type *)e, (const type *)c) < 0) {      \
                sort_swap(c, e);                                               \
              }                                                                \
            }                                                                  \
                                                                               \
            if (compare((const type *)b, (const type *)a) < 0) {               \
              sort_swap(a, b);                                                 \
            }                                                                  \
            if (compare((const type *)e, (const type *)d) < 0) {               \
              sort_swap(d, e);                                                 \
            }                                                                  \
          }                                                                    \
          top--;                                                               \
          base = (type *)top->base;                                            \
          if (!base)                                                           \
            return;                                                            \
          num_elements = top->num_elements;                                    \
        } else {                                                               \
          mid = base + (num_elements >> 1);                                    \
          high = base + (num_elements - 1);                                    \
          if (compare((const type *)mid, (const type *)base) < 0) {            \
            sort_swap(base, mid);                                              \
          }                                                                    \
          if (compare((const type *)high, (const type *)mid) < 0) {            \
            if (compare((const type *)high, (const type *)base) < 0) {         \
              sort_swap(base, high);                                           \
            }                                                                  \
          } else {                                                             \
            sort_swap(mid, high);                                              \
          }                                                                    \
          a = b = base;                                                        \
          while (a < high) {                                                   \
            if (compare((const type *)a, (const type *)high) < 0) {            \
              sort_swap(a, b);                                                 \
              b++;                                                             \
            }                                                                  \
            a++;                                                               \
          }                                                                    \
          sort_swap(b, high);                                                  \
          pivot = b - base;                                                    \
          if ((pivot << 1) < num_elements) {                                   \
            if (pivot == 0 &&                                                  \
                !(compare((const type *)base, (const type *)base + 1))) {      \
              a = base + 2;                                                    \
              while (a <= high &&                                              \
                     !(compare((const type *)base, (const type *)a)))          \
                a++;                                                           \
              num_elements -= (a - base);                                      \
              base = a;                                                        \
            } else {                                                           \
              top->base = base + pivot + 1;                                    \
              top->num_elements = num_elements - (pivot + 1);                  \
              num_elements = pivot;                                            \
              top++;                                                           \
            }                                                                  \
          } else {                                                             \
            top->base = base;                                                  \
            top->num_elements = pivot;                                         \
            base += pivot + 1;                                                 \
            num_elements -= (pivot + 1);                                       \
            top++;                                                             \
          }                                                                    \
        }                                                                      \
      } else {                                                                 \
        a = base;                                                              \
        high = e = base + num_elements - 1;                                    \
        pivot = num_elements >> 1;                                             \
        c = a + pivot;                                                         \
        if (compare((const type *)c, (const type *)a) < 0) {                   \
          sort_swap(c, a);                                                     \
        }                                                                      \
        if (compare((const type *)e, (const type *)c) < 0) {                   \
          sort_swap(e, c);                                                     \
          if (compare((const type *)c, (const type *)a) < 0) {                 \
            sort_swap(c, a);                                                   \
          }                                                                    \
        }                                                                      \
                                                                               \
        b = a + 1;                                                             \
        d = e - 1;                                                             \
        e = c;                                                                 \
        while (1) {                                                            \
          while (compare((const type *)b, (const type *)c) < 0)                \
            b++;                                                               \
          while (compare((const type *)c, (const type *)d) < 0)                \
            d--;                                                               \
          if (b < d) {                                                         \
            sort_swap(b, d);                                                   \
            if (b == c)                                                        \
              c = d;                                                           \
            else if (d == c)                                                   \
              c = b;                                                           \
            b++;                                                               \
            d--;                                                               \
          } else                                                               \
            break;                                                             \
        }                                                                      \
        if (e != c) {                                                          \
          if (c <= d) {                                                        \
            sort_swap(c, d);                                                   \
            pivot = d - a;                                                     \
          } else {                                                             \
            sort_swap(c, b);                                                   \
            pivot = b - a;                                                     \
          }                                                                    \
        } else {                                                               \
          pivot = c - a;                                                       \
        }                                                                      \
        if ((pivot << 1) < num_elements) {                                     \
          if (pivot == 0 &&                                                    \
              !(compare((const type *)base, (const type *)base + 1))) {        \
            a = base + 2;                                                      \
            while (a <= high &&                                                \
                   !(compare((const type *)base, (const type *)a)))            \
              a++;                                                             \
            num_elements -= (a - base);                                        \
            base = a;                                                          \
          } else {                                                             \
            top->base = base + pivot + 1;                                      \
            top->num_elements = num_elements - (pivot + 1);                    \
            num_elements = pivot;                                              \
            top++;                                                             \
          }                                                                    \
        } else {                                                               \
          top->base = base;                                                    \
          top->num_elements = pivot;                                           \
          base += pivot + 1;                                                   \
          num_elements -= (pivot + 1);                                         \
          top++;                                                               \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  }
