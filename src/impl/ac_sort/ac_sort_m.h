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

// extra space
#define ac_sort_m(name, type, compare)                                         \
  void name(type *base, size_t num_elements) {                                 \
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
      if (compare(e, a) < 0) {                                                 \
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
      if (compare(d, c) < 0) {                                                 \
        pivot = 0;                                                             \
        if (compare(e, d) < 0) {                                               \
          sort_swap(c, e);                                                     \
        } else {                                                               \
          sort_swap(c, d);                                                     \
        }                                                                      \
      } else if (compare(e, c) < 0) {                                          \
        pivot = 0;                                                             \
        sort_swap(c, e);                                                       \
      }                                                                        \
      if (compare(c, a) < 0) {                                                 \
        pivot = 0;                                                             \
        sort_swap(a, c);                                                       \
        if (compare(d, c) < 0) {                                               \
          if (compare(e, d) < 0) {                                             \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (compare(e, c) < 0) {                                        \
          sort_swap(c, e);                                                     \
        }                                                                      \
      }                                                                        \
      if (compare(c, b) < 0) {                                                 \
        pivot = 0;                                                             \
        sort_swap(b, c);                                                       \
        if (compare(d, c) < 0) {                                               \
          if (compare(e, d) < 0) {                                             \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (compare(e, c) < 0) {                                        \
          sort_swap(c, e);                                                     \
        }                                                                      \
      }                                                                        \
                                                                               \
      if (!pivot || compare(b, a) < 0 || compare(c, b) < 0 ||                  \
          compare(d, c) < 0 || compare(e, d) < 0) {                            \
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
            if (compare(mid, a) < 0)                                           \
              break;                                                           \
            a++;                                                               \
            mid++;                                                             \
          }                                                                    \
          if (a >= e)                                                          \
            return;                                                            \
          sort_swap(a, mid);                                                   \
          if (a < c) {                                                         \
            if (compare(c, a) < 0) {                                           \
              sort_swap(c, a);                                                 \
            }                                                                  \
            sort_swap(d, e - 1);                                               \
            b = a;                                                             \
            d = e - 2;                                                         \
          } else {                                                             \
            c = a;                                                             \
            d = e - 1;                                                         \
            b = a - 1;                                                         \
          }                                                                    \
          a = base;                                                            \
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
            if (compare(mid, high) < 0)                                        \
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
        while (compare(b, c) < 0)                                              \
          b++;                                                                 \
        while (compare(c, d) < 0)                                              \
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
        if (pivot == 0 && !(compare(base, base + 1))) {                        \
          a = base + 2;                                                        \
          while (a <= high && !(compare(base, a)))                             \
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
            if (compare(base + 1, base) < 0) {                                 \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
          } else if (num_elements == 3) {                                      \
            if (compare(base + 1, base) < 0) {                                 \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
            if (compare(base + 2, base + 1) < 0) {                             \
              sort_swap(base + 1, base + 2);                                   \
              if (compare(base + 1, base) < 0) {                               \
                sort_swap(base, base + 1);                                     \
              }                                                                \
            }                                                                  \
          } else if (num_elements == 4) {                                      \
            if (compare(base + 1, base) < 0) {                                 \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
            if (compare(base + 3, base + 2) < 0) {                             \
              sort_swap(base + 2, base + 3);                                   \
            }                                                                  \
            if (compare(base + 2, base) < 0) {                                 \
              if (compare(base + 3, base) < 0) {                               \
                sort_swap(base, base + 2);                                     \
                sort_swap(base + 1, base + 3);                                 \
              } else {                                                         \
                sort_swap(base, base + 1);                                     \
                sort_swap(base + 0, base + 2);                                 \
                if (compare(base + 3, base + 2) < 0) {                         \
                  sort_swap(base + 2, base + 3);                               \
                }                                                              \
              }                                                                \
            } else {                                                           \
              if (compare(base + 2, base + 1) < 0) {                           \
                sort_swap(base + 1, base + 2);                                 \
                if (compare(base + 3, base + 2) < 0) {                         \
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
            if (compare(d, c) < 0) {                                           \
              pivot = 0;                                                       \
              if (compare(e, d) < 0) {                                         \
                sort_swap(c, e);                                               \
              } else {                                                         \
                sort_swap(c, d);                                               \
              }                                                                \
            } else if (compare(e, c) < 0) {                                    \
              pivot = 0;                                                       \
              sort_swap(c, e);                                                 \
            }                                                                  \
            if (compare(c, a) < 0) {                                           \
              pivot = 0;                                                       \
              sort_swap(a, c);                                                 \
              if (compare(d, c) < 0) {                                         \
                if (compare(e, d) < 0) {                                       \
                  sort_swap(c, e);                                             \
                } else {                                                       \
                  sort_swap(c, d);                                             \
                }                                                              \
              } else if (compare(e, c) < 0) {                                  \
                sort_swap(c, e);                                               \
              }                                                                \
            }                                                                  \
            if (compare(c, b) < 0) {                                           \
              pivot = 0;                                                       \
              sort_swap(b, c);                                                 \
              if (compare(d, c) < 0) {                                         \
                if (compare(e, d) < 0) {                                       \
                  sort_swap(c, e);                                             \
                } else {                                                       \
                  sort_swap(c, d);                                             \
                }                                                              \
              } else if (compare(e, c) < 0) {                                  \
                sort_swap(c, e);                                               \
              }                                                                \
            }                                                                  \
                                                                               \
            if (compare(b, a) < 0) {                                           \
              sort_swap(a, b);                                                 \
            }                                                                  \
            if (compare(e, d) < 0) {                                           \
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
          if (compare(mid, base) < 0) {                                        \
            sort_swap(base, mid);                                              \
          }                                                                    \
          if (compare(high, mid) < 0) {                                        \
            if (compare(high, base) < 0) {                                     \
              sort_swap(base, high);                                           \
            }                                                                  \
          } else {                                                             \
            sort_swap(mid, high);                                              \
          }                                                                    \
          a = b = base;                                                        \
          while (a < high) {                                                   \
            if (compare(a, high) < 0) {                                        \
              sort_swap(a, b);                                                 \
              b++;                                                             \
            }                                                                  \
            a++;                                                               \
          }                                                                    \
          sort_swap(b, high);                                                  \
          pivot = b - base;                                                    \
          if ((pivot << 1) < num_elements) {                                   \
            if (pivot == 0 && !(compare(base, base + 1))) {                    \
              a = base + 2;                                                    \
              while (a <= high && !(compare(base, a)))                         \
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
        if (compare(c, a) < 0) {                                               \
          sort_swap(c, a);                                                     \
        }                                                                      \
        if (compare(e, c) < 0) {                                               \
          sort_swap(e, c);                                                     \
          if (compare(c, a) < 0) {                                             \
            sort_swap(c, a);                                                   \
          }                                                                    \
        }                                                                      \
                                                                               \
        b = a + 1;                                                             \
        d = e - 1;                                                             \
        e = c;                                                                 \
        while (1) {                                                            \
          while (compare(b, c) < 0)                                            \
            b++;                                                               \
          while (compare(c, d) < 0)                                            \
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
          if (pivot == 0 && !(compare(base, base + 1))) {                      \
            a = base + 2;                                                      \
            while (a <= high && !(compare(base, a)))                           \
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
