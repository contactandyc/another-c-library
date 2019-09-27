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

#include <stdio.h>
#include <stdlib.h>

#define stla_sort_print_m(name, datatype)                                      \
  void name(char *func_line, datatype *base, ssize_t num_elements,             \
            void (*print_element)(datatype * el)) {                            \
    if (func_line)                                                             \
      printf("%s ", func_line);                                                \
    printf("%ld elements\n", num_elements);                                    \
    datatype *ep = base + num_elements;                                        \
    while (base < ep) {                                                        \
      print_element(base);                                                     \
      base++;                                                                  \
    }                                                                          \
    printf("\n");                                                              \
  }

#define stla_sort_print_arg_m(name, datatype)                                  \
  void name(char *func_line, datatype *base, ssize_t num_elements,             \
            void (*print_element)(datatype * el, void *arg), void *arg) {      \
    if (func_line)                                                             \
      printf("%s ", func_line);                                                \
    printf("%ld elements\n", num_elements);                                    \
    datatype *ep = base + num_elements;                                        \
    while (base < ep) {                                                        \
      print_element(base, arg);                                                \
      base++;                                                                  \
    }                                                                          \
    printf("\n");                                                              \
  }

#define stla_sort_test_m(name, less, datatype)                                 \
  void name(char *func_line, datatype *base, ssize_t num_elements,             \
            void (*print_element)(datatype * el)) {                            \
    datatype *ep = base + num_elements;                                        \
    base++;                                                                    \
    int n;                                                                     \
    while (base < ep) {                                                        \
      if (less(base - 1, base))                                                \
        if (n > 0) {                                                           \
          if (func_line)                                                       \
            printf("%s ", func_line);                                          \
          print_element(base - 1);                                             \
          printf(" is greater than ");                                         \
          print_element(base);                                                 \
          printf("\n");                                                        \
          abort();                                                             \
        }                                                                      \
      base++;                                                                  \
    }                                                                          \
  }

#define stla_sort_test_arg_m(name, less, datatype)                             \
  void name(char *func_line, datatype *base, ssize_t num_elements,             \
            void (*print_element)(datatype * el, void *arg), void *arg) {      \
    datatype *ep = base + num_elements;                                        \
    base++;                                                                    \
    int n;                                                                     \
    while (base < ep) {                                                        \
      if (less(base - 1, base, arg))                                           \
        if (n > 0) {                                                           \
          if (func_line)                                                       \
            printf("%s ", func_line);                                          \
          print_element(base - 1, arg);                                        \
          printf(" is greater than ");                                         \
          print_element(base, arg);                                            \
          printf("\n");                                                        \
          abort();                                                             \
        }                                                                      \
      base++;                                                                  \
    }                                                                          \
  }

#define stla_sort_test_less_m(name, datatype)                                  \
  void name(char *func_line, datatype *base, ssize_t num_elements,             \
            bool (*less)(datatype * a, datatype * b),                          \
            void (*print_element)(datatype * el)) {                            \
    datatype *ep = base + num_elements;                                        \
    base++;                                                                    \
    int n;                                                                     \
    while (base < ep) {                                                        \
      if (less(base - 1, base))                                                \
        if (n > 0) {                                                           \
          if (func_line)                                                       \
            printf("%s ", func_line);                                          \
          print_element(base - 1);                                             \
          printf(" is greater than ");                                         \
          print_element(base);                                                 \
          printf("\n");                                                        \
          abort();                                                             \
        }                                                                      \
      base++;                                                                  \
    }                                                                          \
  }

#define stla_sort_test_less_arg_m(name, datatype)                              \
  void name(char *func_line, datatype *base, ssize_t num_elements,             \
            bool (*less)(datatype * a, datatype * b, void *arg),               \
            void (*print_element)(datatype * el, void *arg), void *arg) {      \
    datatype *ep = base + num_elements;                                        \
    base++;                                                                    \
    int n;                                                                     \
    while (base < ep) {                                                        \
      if (less(base - 1, base, arg))                                           \
        if (n > 0) {                                                           \
          if (func_line)                                                       \
            printf("%s ", func_line);                                          \
          print_element(base - 1, arg);                                        \
          printf(" is greater than ");                                         \
          print_element(base, arg);                                            \
          printf("\n");                                                        \
          abort();                                                             \
        }                                                                      \
      base++;                                                                  \
    }                                                                          \
  }

#define sort_swap(a, b)                                                        \
  tmp = *(a);                                                                  \
  *(a) = *(b);                                                                 \
  *(b) = tmp

typedef struct {
  void *base;
  ssize_t num_elements;
} sort_stack_t;

#define stla_sort_m(name, type, less)                                          \
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
      e = base + num_elements - 1;                                             \
      pivot = num_elements >> 1;                                               \
      c = a + pivot;                                                           \
      if (less(e, a)) {                                                        \
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
      if (less(d, c)) {                                                        \
        pivot = 0;                                                             \
        if (less(e, d)) {                                                      \
          sort_swap(c, e);                                                     \
        } else {                                                               \
          sort_swap(c, d);                                                     \
        }                                                                      \
      } else if (less(e, c)) {                                                 \
        pivot = 0;                                                             \
        sort_swap(c, e);                                                       \
      }                                                                        \
      if (less(c, a)) {                                                        \
        pivot = 0;                                                             \
        sort_swap(a, c);                                                       \
        if (less(d, c)) {                                                      \
          if (less(e, d)) {                                                    \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (less(e, c)) {                                               \
          sort_swap(c, e);                                                     \
        }                                                                      \
      }                                                                        \
      if (less(c, b)) {                                                        \
        pivot = 0;                                                             \
        sort_swap(b, c);                                                       \
        if (less(d, c)) {                                                      \
          if (less(e, d)) {                                                    \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (less(e, c)) {                                               \
          sort_swap(c, e);                                                     \
        }                                                                      \
      }                                                                        \
                                                                               \
      if (!pivot || less(b, a) || less(c, b) || less(d, c) || less(e, a)) {    \
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
      } else {                                                                 \
        if (pivot == 1) {                                                      \
          mid = a + 1;                                                         \
          while (a < e) {                                                      \
            if (less(mid, a))                                                  \
              break;                                                           \
            a++;                                                               \
            mid++;                                                             \
          }                                                                    \
          if (a >= e)                                                          \
            return;                                                            \
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
            if (less(mid, high))                                               \
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
          }                                                                    \
        }                                                                      \
      }                                                                        \
                                                                               \
      sort_swap(b, a + 1);                                                     \
      sort_swap(d, e - 1);                                                     \
      b = a + 2;                                                               \
      d = e - 2;                                                               \
      e = c;                                                                   \
      while (1) {                                                              \
        while (less(b, c))                                                     \
          b++;                                                                 \
        while (less(c, d))                                                     \
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
        if (pivot == 0 && !(less(base, base + 1))) {                           \
          a = base + 2;                                                        \
          while (a <= high && !(less(base, a)))                                \
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
            if (less(base + 1, base)) {                                        \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
          } else if (num_elements == 3) {                                      \
            if (less(base + 1, base)) {                                        \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
            if (less(base + 2, base + 1)) {                                    \
              sort_swap(base + 1, base + 2);                                   \
              if (less(base + 1, base)) {                                      \
                sort_swap(base, base + 1);                                     \
              }                                                                \
            }                                                                  \
          } else if (num_elements == 4) {                                      \
            if (less(base + 1, base)) {                                        \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
            if (less(base + 3, base + 2)) {                                    \
              sort_swap(base + 2, base + 3);                                   \
            }                                                                  \
            if (less(base + 2, base)) {                                        \
              if (less(base + 3, base)) {                                      \
                sort_swap(base, base + 2);                                     \
                sort_swap(base + 1, base + 3);                                 \
              } else {                                                         \
                sort_swap(base, base + 1);                                     \
                sort_swap(base + 0, base + 2);                                 \
                if (less(base + 3, base + 2)) {                                \
                  sort_swap(base + 2, base + 3);                               \
                }                                                              \
              }                                                                \
            } else {                                                           \
              if (less(base + 2, base + 1)) {                                  \
                sort_swap(base + 1, base + 2);                                 \
                if (less(base + 3, base + 2)) {                                \
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
            if (less(d, c)) {                                                  \
              pivot = 0;                                                       \
              if (less(e, d)) {                                                \
                sort_swap(c, e);                                               \
              } else {                                                         \
                sort_swap(c, d);                                               \
              }                                                                \
            } else if (less(e, c)) {                                           \
              pivot = 0;                                                       \
              sort_swap(c, e);                                                 \
            }                                                                  \
            if (less(c, a)) {                                                  \
              pivot = 0;                                                       \
              sort_swap(a, c);                                                 \
              if (less(d, c)) {                                                \
                if (less(e, d)) {                                              \
                  sort_swap(c, e);                                             \
                } else {                                                       \
                  sort_swap(c, d);                                             \
                }                                                              \
              } else if (less(e, c)) {                                         \
                sort_swap(c, e);                                               \
              }                                                                \
            }                                                                  \
            if (less(c, b)) {                                                  \
              pivot = 0;                                                       \
              sort_swap(b, c);                                                 \
              if (less(d, c)) {                                                \
                if (less(e, d)) {                                              \
                  sort_swap(c, e);                                             \
                } else {                                                       \
                  sort_swap(c, d);                                             \
                }                                                              \
              } else if (less(e, c)) {                                         \
                sort_swap(c, e);                                               \
              }                                                                \
            }                                                                  \
                                                                               \
            if (less(b, a)) {                                                  \
              sort_swap(a, b);                                                 \
            }                                                                  \
            if (less(e, d)) {                                                  \
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
          if (less(mid, base)) {                                               \
            sort_swap(base, mid);                                              \
          }                                                                    \
          if (less(high, mid)) {                                               \
            if (less(high, base)) {                                            \
              sort_swap(base, high);                                           \
            }                                                                  \
          } else {                                                             \
            sort_swap(mid, high);                                              \
          }                                                                    \
          a = b = base;                                                        \
          while (a < high) {                                                   \
            if (less(a, high)) {                                               \
              sort_swap(a, b);                                                 \
              b++;                                                             \
            }                                                                  \
            a++;                                                               \
          }                                                                    \
          sort_swap(b, high);                                                  \
          pivot = b - base;                                                    \
                                                                               \
          if ((pivot << 1) < num_elements) {                                   \
            if (pivot == 0 && !(less(base, base + 1))) {                       \
              a = base + 2;                                                    \
              while (a <= high && !(less(base, a)))                            \
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
        e = base + num_elements - 1;                                           \
        pivot = num_elements >> 1;                                             \
        c = a + pivot;                                                         \
        pivot >>= 1;                                                           \
        b = a + pivot;                                                         \
        d = c + pivot;                                                         \
        pivot = 1;                                                             \
                                                                               \
        if (less(d, c)) {                                                      \
          pivot = 0;                                                           \
          if (less(e, d)) {                                                    \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (less(e, c)) {                                               \
          pivot = 0;                                                           \
          sort_swap(c, e);                                                     \
        }                                                                      \
        if (less(c, a)) {                                                      \
          pivot = 0;                                                           \
          sort_swap(a, c);                                                     \
          if (less(d, c)) {                                                    \
            if (less(e, d)) {                                                  \
              sort_swap(c, e);                                                 \
            } else {                                                           \
              sort_swap(c, d);                                                 \
            }                                                                  \
          } else if (less(e, c)) {                                             \
            sort_swap(c, e);                                                   \
          }                                                                    \
        }                                                                      \
        if (less(c, b)) {                                                      \
          pivot = 0;                                                           \
          sort_swap(b, c);                                                     \
          if (less(d, c)) {                                                    \
            if (less(e, d)) {                                                  \
              sort_swap(c, e);                                                 \
            } else {                                                           \
              sort_swap(c, d);                                                 \
            }                                                                  \
          } else if (less(e, c)) {                                             \
            sort_swap(c, e);                                                   \
          }                                                                    \
        }                                                                      \
                                                                               \
        sort_swap(b, a + 1);                                                   \
        sort_swap(d, e - 1);                                                   \
        b = a + 2;                                                             \
        d = e - 2;                                                             \
        e = c;                                                                 \
        while (1) {                                                            \
          while (less(b, c))                                                   \
            b++;                                                               \
          while (less(c, d))                                                   \
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
          if (pivot == 0 && !(less(base, base + 1))) {                         \
            a = base + 2;                                                      \
            while (a <= high && !(less(base, a)))                              \
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

#define stla_sort_arg_m(name, type, less)                                      \
  void name(type *base, size_t num_elements, void *arg) {                      \
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
      e = base + num_elements - 1;                                             \
      pivot = num_elements >> 1;                                               \
      c = a + pivot;                                                           \
      if (less(e, a, arg)) {                                                   \
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
      if (less(d, c, arg)) {                                                   \
        pivot = 0;                                                             \
        if (less(e, d, arg)) {                                                 \
          sort_swap(c, e);                                                     \
        } else {                                                               \
          sort_swap(c, d);                                                     \
        }                                                                      \
      } else if (less(e, c, arg)) {                                            \
        pivot = 0;                                                             \
        sort_swap(c, e);                                                       \
      }                                                                        \
      if (less(c, a, arg)) {                                                   \
        pivot = 0;                                                             \
        sort_swap(a, c);                                                       \
        if (less(d, c, arg)) {                                                 \
          if (less(e, d, arg)) {                                               \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (less(e, c, arg)) {                                          \
          sort_swap(c, e);                                                     \
        }                                                                      \
      }                                                                        \
      if (less(c, b, arg)) {                                                   \
        pivot = 0;                                                             \
        sort_swap(b, c);                                                       \
        if (less(d, c, arg)) {                                                 \
          if (less(e, d, arg)) {                                               \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (less(e, c, arg)) {                                          \
          sort_swap(c, e);                                                     \
        }                                                                      \
      }                                                                        \
                                                                               \
      if (!pivot || less(b, a, arg) || less(c, b, arg) || less(d, c, arg) ||   \
          less(e, a, arg)) {                                                   \
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
      } else {                                                                 \
        if (pivot == 1) {                                                      \
          mid = a + 1;                                                         \
          while (a < e) {                                                      \
            if (less(mid, a, arg))                                             \
              break;                                                           \
            a++;                                                               \
            mid++;                                                             \
          }                                                                    \
          if (a >= e)                                                          \
            return;                                                            \
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
            if (less(mid, high, arg))                                          \
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
          }                                                                    \
        }                                                                      \
      }                                                                        \
                                                                               \
      sort_swap(b, a + 1);                                                     \
      sort_swap(d, e - 1);                                                     \
      b = a + 2;                                                               \
      d = e - 2;                                                               \
      e = c;                                                                   \
      while (1) {                                                              \
        while (less(b, c, arg))                                                \
          b++;                                                                 \
        while (less(c, d, arg))                                                \
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
        if (pivot == 0 && !(less(base, base + 1, arg))) {                      \
          a = base + 2;                                                        \
          while (a <= high && !(less(base, a, arg)))                           \
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
            if (less(base + 1, base, arg)) {                                   \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
          } else if (num_elements == 3) {                                      \
            if (less(base + 1, base, arg)) {                                   \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
            if (less(base + 2, base + 1, arg)) {                               \
              sort_swap(base + 1, base + 2);                                   \
              if (less(base + 1, base, arg)) {                                 \
                sort_swap(base, base + 1);                                     \
              }                                                                \
            }                                                                  \
          } else if (num_elements == 4) {                                      \
            if (less(base + 1, base, arg)) {                                   \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
            if (less(base + 3, base + 2, arg)) {                               \
              sort_swap(base + 2, base + 3);                                   \
            }                                                                  \
            if (less(base + 2, base, arg)) {                                   \
              if (less(base + 3, base, arg)) {                                 \
                sort_swap(base, base + 2);                                     \
                sort_swap(base + 1, base + 3);                                 \
              } else {                                                         \
                sort_swap(base, base + 1);                                     \
                sort_swap(base + 0, base + 2);                                 \
                if (less(base + 3, base + 2, arg)) {                           \
                  sort_swap(base + 2, base + 3);                               \
                }                                                              \
              }                                                                \
            } else {                                                           \
              if (less(base + 2, base + 1, arg)) {                             \
                sort_swap(base + 1, base + 2);                                 \
                if (less(base + 3, base + 2, arg)) {                           \
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
            if (less(d, c, arg)) {                                             \
              pivot = 0;                                                       \
              if (less(e, d, arg)) {                                           \
                sort_swap(c, e);                                               \
              } else {                                                         \
                sort_swap(c, d);                                               \
              }                                                                \
            } else if (less(e, c, arg)) {                                      \
              pivot = 0;                                                       \
              sort_swap(c, e);                                                 \
            }                                                                  \
            if (less(c, a, arg)) {                                             \
              pivot = 0;                                                       \
              sort_swap(a, c);                                                 \
              if (less(d, c, arg)) {                                           \
                if (less(e, d, arg)) {                                         \
                  sort_swap(c, e);                                             \
                } else {                                                       \
                  sort_swap(c, d);                                             \
                }                                                              \
              } else if (less(e, c, arg)) {                                    \
                sort_swap(c, e);                                               \
              }                                                                \
            }                                                                  \
            if (less(c, b, arg)) {                                             \
              pivot = 0;                                                       \
              sort_swap(b, c);                                                 \
              if (less(d, c, arg)) {                                           \
                if (less(e, d, arg)) {                                         \
                  sort_swap(c, e);                                             \
                } else {                                                       \
                  sort_swap(c, d);                                             \
                }                                                              \
              } else if (less(e, c, arg)) {                                    \
                sort_swap(c, e);                                               \
              }                                                                \
            }                                                                  \
                                                                               \
            if (less(b, a, arg)) {                                             \
              sort_swap(a, b);                                                 \
            }                                                                  \
            if (less(e, d, arg)) {                                             \
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
          if (less(mid, base, arg)) {                                          \
            sort_swap(base, mid);                                              \
          }                                                                    \
          if (less(high, mid, arg)) {                                          \
            if (less(high, base, arg)) {                                       \
              sort_swap(base, high);                                           \
            }                                                                  \
          } else {                                                             \
            sort_swap(mid, high);                                              \
          }                                                                    \
          a = b = base;                                                        \
          while (a < high) {                                                   \
            if (less(a, high, arg)) {                                          \
              sort_swap(a, b);                                                 \
              b++;                                                             \
            }                                                                  \
            a++;                                                               \
          }                                                                    \
          sort_swap(b, high);                                                  \
          pivot = b - base;                                                    \
                                                                               \
          if ((pivot << 1) < num_elements) {                                   \
            if (pivot == 0 && !(less(base, base + 1, arg))) {                  \
              a = base + 2;                                                    \
              while (a <= high && !(less(base, a, arg)))                       \
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
        e = base + num_elements - 1;                                           \
        pivot = num_elements >> 1;                                             \
        c = a + pivot;                                                         \
        pivot >>= 1;                                                           \
        b = a + pivot;                                                         \
        d = c + pivot;                                                         \
        pivot = 1;                                                             \
                                                                               \
        if (less(d, c, arg)) {                                                 \
          pivot = 0;                                                           \
          if (less(e, d, arg)) {                                               \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (less(e, c, arg)) {                                          \
          pivot = 0;                                                           \
          sort_swap(c, e);                                                     \
        }                                                                      \
        if (less(c, a, arg)) {                                                 \
          pivot = 0;                                                           \
          sort_swap(a, c);                                                     \
          if (less(d, c, arg)) {                                               \
            if (less(e, d, arg)) {                                             \
              sort_swap(c, e);                                                 \
            } else {                                                           \
              sort_swap(c, d);                                                 \
            }                                                                  \
          } else if (less(e, c, arg)) {                                        \
            sort_swap(c, e);                                                   \
          }                                                                    \
        }                                                                      \
        if (less(c, b, arg)) {                                                 \
          pivot = 0;                                                           \
          sort_swap(b, c);                                                     \
          if (less(d, c, arg)) {                                               \
            if (less(e, d, arg)) {                                             \
              sort_swap(c, e);                                                 \
            } else {                                                           \
              sort_swap(c, d);                                                 \
            }                                                                  \
          } else if (less(e, c, arg)) {                                        \
            sort_swap(c, e);                                                   \
          }                                                                    \
        }                                                                      \
                                                                               \
        sort_swap(b, a + 1);                                                   \
        sort_swap(d, e - 1);                                                   \
        b = a + 2;                                                             \
        d = e - 2;                                                             \
        e = c;                                                                 \
        while (1) {                                                            \
          while (less(b, c, arg))                                              \
            b++;                                                               \
          while (less(c, d, arg))                                              \
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
          if (pivot == 0 && !(less(base, base + 1, arg))) {                    \
            a = base + 2;                                                      \
            while (a <= high && !(less(base, a, arg)))                         \
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

#define stla_sort_less_m(name, type)                                           \
  void name(type *base, bool (*less)(datatype * a, datatype * b),              \
            size_t num_elements) {                                             \
    type *a, *b, *c, *d, *e;                                                   \
    type *mid, *high, tmp;                                                     \
    ssize_t pivot;                                                             \
    sort_stack_t stack[64];                                                    \
    sort_stack_t *top = stack;                                                 \
    top->base = NULL;                                                          \
    top++;                                                                     \
    if (num_elements > 12) {                                                   \
      a = base;                                                                \
      e = base + num_elements - 1;                                             \
      pivot = num_elements >> 1;                                               \
      c = a + pivot;                                                           \
      if (less(e, a)) {                                                        \
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
      if (less(d, c)) {                                                        \
        pivot = 0;                                                             \
        if (less(e, d)) {                                                      \
          sort_swap(c, e);                                                     \
        } else {                                                               \
          sort_swap(c, d);                                                     \
        }                                                                      \
      } else if (less(e, c)) {                                                 \
        pivot = 0;                                                             \
        sort_swap(c, e);                                                       \
      }                                                                        \
      if (less(c, a)) {                                                        \
        pivot = 0;                                                             \
        sort_swap(a, c);                                                       \
        if (less(d, c)) {                                                      \
          if (less(e, d)) {                                                    \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (less(e, c)) {                                               \
          sort_swap(c, e);                                                     \
        }                                                                      \
      }                                                                        \
      if (less(c, b)) {                                                        \
        pivot = 0;                                                             \
        sort_swap(b, c);                                                       \
        if (less(d, c)) {                                                      \
          if (less(e, d)) {                                                    \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (less(e, c)) {                                               \
          sort_swap(c, e);                                                     \
        }                                                                      \
      }                                                                        \
                                                                               \
      if (!pivot || less(b, a) || less(c, b) || less(d, c) || less(e, a)) {    \
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
      } else {                                                                 \
        if (pivot == 1) {                                                      \
          mid = a + 1;                                                         \
          while (a < e) {                                                      \
            if (less(mid, a))                                                  \
              break;                                                           \
            a++;                                                               \
            mid++;                                                             \
          }                                                                    \
          if (a >= e)                                                          \
            return;                                                            \
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
            if (less(mid, high))                                               \
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
          }                                                                    \
        }                                                                      \
      }                                                                        \
                                                                               \
      sort_swap(b, a + 1);                                                     \
      sort_swap(d, e - 1);                                                     \
      b = a + 2;                                                               \
      d = e - 2;                                                               \
      e = c;                                                                   \
      while (1) {                                                              \
        while (less(b, c))                                                     \
          b++;                                                                 \
        while (less(c, d))                                                     \
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
        if (pivot == 0 && !(less(base, base + 1))) {                           \
          a = base + 2;                                                        \
          while (a <= high && !(less(base, a)))                                \
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
            if (less(base + 1, base)) {                                        \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
          } else if (num_elements == 3) {                                      \
            if (less(base + 1, base)) {                                        \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
            if (less(base + 2, base + 1)) {                                    \
              sort_swap(base + 1, base + 2);                                   \
              if (less(base + 1, base)) {                                      \
                sort_swap(base, base + 1);                                     \
              }                                                                \
            }                                                                  \
          } else if (num_elements == 4) {                                      \
            if (less(base + 1, base)) {                                        \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
            if (less(base + 3, base + 2)) {                                    \
              sort_swap(base + 2, base + 3);                                   \
            }                                                                  \
            if (less(base + 2, base)) {                                        \
              if (less(base + 3, base)) {                                      \
                sort_swap(base, base + 2);                                     \
                sort_swap(base + 1, base + 3);                                 \
              } else {                                                         \
                sort_swap(base, base + 1);                                     \
                sort_swap(base + 0, base + 2);                                 \
                if (less(base + 3, base + 2)) {                                \
                  sort_swap(base + 2, base + 3);                               \
                }                                                              \
              }                                                                \
            } else {                                                           \
              if (less(base + 2, base + 1)) {                                  \
                sort_swap(base + 1, base + 2);                                 \
                if (less(base + 3, base + 2)) {                                \
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
            if (less(d, c)) {                                                  \
              pivot = 0;                                                       \
              if (less(e, d)) {                                                \
                sort_swap(c, e);                                               \
              } else {                                                         \
                sort_swap(c, d);                                               \
              }                                                                \
            } else if (less(e, c)) {                                           \
              pivot = 0;                                                       \
              sort_swap(c, e);                                                 \
            }                                                                  \
            if (less(c, a)) {                                                  \
              pivot = 0;                                                       \
              sort_swap(a, c);                                                 \
              if (less(d, c)) {                                                \
                if (less(e, d)) {                                              \
                  sort_swap(c, e);                                             \
                } else {                                                       \
                  sort_swap(c, d);                                             \
                }                                                              \
              } else if (less(e, c)) {                                         \
                sort_swap(c, e);                                               \
              }                                                                \
            }                                                                  \
            if (less(c, b)) {                                                  \
              pivot = 0;                                                       \
              sort_swap(b, c);                                                 \
              if (less(d, c)) {                                                \
                if (less(e, d)) {                                              \
                  sort_swap(c, e);                                             \
                } else {                                                       \
                  sort_swap(c, d);                                             \
                }                                                              \
              } else if (less(e, c)) {                                         \
                sort_swap(c, e);                                               \
              }                                                                \
            }                                                                  \
                                                                               \
            if (less(b, a)) {                                                  \
              sort_swap(a, b);                                                 \
            }                                                                  \
            if (less(e, d)) {                                                  \
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
          if (less(mid, base)) {                                               \
            sort_swap(base, mid);                                              \
          }                                                                    \
          if (less(high, mid)) {                                               \
            if (less(high, base)) {                                            \
              sort_swap(base, high);                                           \
            }                                                                  \
          } else {                                                             \
            sort_swap(mid, high);                                              \
          }                                                                    \
          a = b = base;                                                        \
          while (a < high) {                                                   \
            if (less(a, high)) {                                               \
              sort_swap(a, b);                                                 \
              b++;                                                             \
            }                                                                  \
            a++;                                                               \
          }                                                                    \
          sort_swap(b, high);                                                  \
          pivot = b - base;                                                    \
                                                                               \
          if ((pivot << 1) < num_elements) {                                   \
            if (pivot == 0 && !(less(base, base + 1))) {                       \
              a = base + 2;                                                    \
              while (a <= high && !(less(base, a)))                            \
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
        e = base + num_elements - 1;                                           \
        pivot = num_elements >> 1;                                             \
        c = a + pivot;                                                         \
        pivot >>= 1;                                                           \
        b = a + pivot;                                                         \
        d = c + pivot;                                                         \
        pivot = 1;                                                             \
                                                                               \
        if (less(d, c)) {                                                      \
          pivot = 0;                                                           \
          if (less(e, d)) {                                                    \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (less(e, c)) {                                               \
          pivot = 0;                                                           \
          sort_swap(c, e);                                                     \
        }                                                                      \
        if (less(c, a)) {                                                      \
          pivot = 0;                                                           \
          sort_swap(a, c);                                                     \
          if (less(d, c)) {                                                    \
            if (less(e, d)) {                                                  \
              sort_swap(c, e);                                                 \
            } else {                                                           \
              sort_swap(c, d);                                                 \
            }                                                                  \
          } else if (less(e, c)) {                                             \
            sort_swap(c, e);                                                   \
          }                                                                    \
        }                                                                      \
        if (less(c, b)) {                                                      \
          pivot = 0;                                                           \
          sort_swap(b, c);                                                     \
          if (less(d, c)) {                                                    \
            if (less(e, d)) {                                                  \
              sort_swap(c, e);                                                 \
            } else {                                                           \
              sort_swap(c, d);                                                 \
            }                                                                  \
          } else if (less(e, c)) {                                             \
            sort_swap(c, e);                                                   \
          }                                                                    \
        }                                                                      \
                                                                               \
        sort_swap(b, a + 1);                                                   \
        sort_swap(d, e - 1);                                                   \
        b = a + 2;                                                             \
        d = e - 2;                                                             \
        e = c;                                                                 \
        while (1) {                                                            \
          while (less(b, c))                                                   \
            b++;                                                               \
          while (less(c, d))                                                   \
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
          if (pivot == 0 && !(less(base, base + 1))) {                         \
            a = base + 2;                                                      \
            while (a <= high && !(less(base, a)))                              \
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

#define stla_sort_less_arg_m(name, type)                                       \
  void name(type *base, size_t num_elements,                                   \
            bool (*less)(datatype * a, datatype * b, void *arg), void *arg) {  \
    type *a, *b, *c, *d, *e;                                                   \
    type *mid, *high, tmp;                                                     \
    ssize_t pivot;                                                             \
    sort_stack_t stack[64];                                                    \
    sort_stack_t *top = stack;                                                 \
    top->base = NULL;                                                          \
    top++;                                                                     \
    if (num_elements > 12) {                                                   \
      a = base;                                                                \
      e = base + num_elements - 1;                                             \
      pivot = num_elements >> 1;                                               \
      c = a + pivot;                                                           \
      if (less(e, a, arg)) {                                                   \
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
      if (less(d, c, arg)) {                                                   \
        pivot = 0;                                                             \
        if (less(e, d, arg)) {                                                 \
          sort_swap(c, e);                                                     \
        } else {                                                               \
          sort_swap(c, d);                                                     \
        }                                                                      \
      } else if (less(e, c, arg)) {                                            \
        pivot = 0;                                                             \
        sort_swap(c, e);                                                       \
      }                                                                        \
      if (less(c, a, arg)) {                                                   \
        pivot = 0;                                                             \
        sort_swap(a, c);                                                       \
        if (less(d, c, arg)) {                                                 \
          if (less(e, d, arg)) {                                               \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (less(e, c, arg)) {                                          \
          sort_swap(c, e);                                                     \
        }                                                                      \
      }                                                                        \
      if (less(c, b, arg)) {                                                   \
        pivot = 0;                                                             \
        sort_swap(b, c);                                                       \
        if (less(d, c, arg)) {                                                 \
          if (less(e, d, arg)) {                                               \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (less(e, c, arg)) {                                          \
          sort_swap(c, e);                                                     \
        }                                                                      \
      }                                                                        \
                                                                               \
      if (!pivot || less(b, a, arg) || less(c, b, arg) || less(d, c, arg) ||   \
          less(e, a, arg)) {                                                   \
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
      } else {                                                                 \
        if (pivot == 1) {                                                      \
          mid = a + 1;                                                         \
          while (a < e) {                                                      \
            if (less(mid, a, arg))                                             \
              break;                                                           \
            a++;                                                               \
            mid++;                                                             \
          }                                                                    \
          if (a >= e)                                                          \
            return;                                                            \
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
            if (less(mid, high, arg))                                          \
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
          }                                                                    \
        }                                                                      \
      }                                                                        \
                                                                               \
      sort_swap(b, a + 1);                                                     \
      sort_swap(d, e - 1);                                                     \
      b = a + 2;                                                               \
      d = e - 2;                                                               \
      e = c;                                                                   \
      while (1) {                                                              \
        while (less(b, c, arg))                                                \
          b++;                                                                 \
        while (less(c, d, arg))                                                \
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
        if (pivot == 0 && !(less(base, base + 1, arg))) {                      \
          a = base + 2;                                                        \
          while (a <= high && !(less(base, a, arg)))                           \
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
            if (less(base + 1, base, arg)) {                                   \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
          } else if (num_elements == 3) {                                      \
            if (less(base + 1, base, arg)) {                                   \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
            if (less(base + 2, base + 1, arg)) {                               \
              sort_swap(base + 1, base + 2);                                   \
              if (less(base + 1, base, arg)) {                                 \
                sort_swap(base, base + 1);                                     \
              }                                                                \
            }                                                                  \
          } else if (num_elements == 4) {                                      \
            if (less(base + 1, base, arg)) {                                   \
              sort_swap(base, base + 1);                                       \
            }                                                                  \
            if (less(base + 3, base + 2, arg)) {                               \
              sort_swap(base + 2, base + 3);                                   \
            }                                                                  \
            if (less(base + 2, base, arg)) {                                   \
              if (less(base + 3, base, arg)) {                                 \
                sort_swap(base, base + 2);                                     \
                sort_swap(base + 1, base + 3);                                 \
              } else {                                                         \
                sort_swap(base, base + 1);                                     \
                sort_swap(base + 0, base + 2);                                 \
                if (less(base + 3, base + 2, arg)) {                           \
                  sort_swap(base + 2, base + 3);                               \
                }                                                              \
              }                                                                \
            } else {                                                           \
              if (less(base + 2, base + 1, arg)) {                             \
                sort_swap(base + 1, base + 2);                                 \
                if (less(base + 3, base + 2, arg)) {                           \
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
            if (less(d, c, arg)) {                                             \
              pivot = 0;                                                       \
              if (less(e, d, arg)) {                                           \
                sort_swap(c, e);                                               \
              } else {                                                         \
                sort_swap(c, d);                                               \
              }                                                                \
            } else if (less(e, c, arg)) {                                      \
              pivot = 0;                                                       \
              sort_swap(c, e);                                                 \
            }                                                                  \
            if (less(c, a, arg)) {                                             \
              pivot = 0;                                                       \
              sort_swap(a, c);                                                 \
              if (less(d, c, arg)) {                                           \
                if (less(e, d, arg)) {                                         \
                  sort_swap(c, e);                                             \
                } else {                                                       \
                  sort_swap(c, d);                                             \
                }                                                              \
              } else if (less(e, c, arg)) {                                    \
                sort_swap(c, e);                                               \
              }                                                                \
            }                                                                  \
            if (less(c, b, arg)) {                                             \
              pivot = 0;                                                       \
              sort_swap(b, c);                                                 \
              if (less(d, c, arg)) {                                           \
                if (less(e, d, arg)) {                                         \
                  sort_swap(c, e);                                             \
                } else {                                                       \
                  sort_swap(c, d);                                             \
                }                                                              \
              } else if (less(e, c, arg)) {                                    \
                sort_swap(c, e);                                               \
              }                                                                \
            }                                                                  \
                                                                               \
            if (less(b, a, arg)) {                                             \
              sort_swap(a, b);                                                 \
            }                                                                  \
            if (less(e, d, arg)) {                                             \
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
          if (less(mid, base, arg)) {                                          \
            sort_swap(base, mid);                                              \
          }                                                                    \
          if (less(high, mid, arg)) {                                          \
            if (less(high, base, arg)) {                                       \
              sort_swap(base, high);                                           \
            }                                                                  \
          } else {                                                             \
            sort_swap(mid, high);                                              \
          }                                                                    \
          a = b = base;                                                        \
          while (a < high) {                                                   \
            if (less(a, high, arg)) {                                          \
              sort_swap(a, b);                                                 \
              b++;                                                             \
            }                                                                  \
            a++;                                                               \
          }                                                                    \
          sort_swap(b, high);                                                  \
          pivot = b - base;                                                    \
                                                                               \
          if ((pivot << 1) < num_elements) {                                   \
            if (pivot == 0 && !(less(base, base + 1, arg))) {                  \
              a = base + 2;                                                    \
              while (a <= high && !(less(base, a, arg)))                       \
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
        e = base + num_elements - 1;                                           \
        pivot = num_elements >> 1;                                             \
        c = a + pivot;                                                         \
        pivot >>= 1;                                                           \
        b = a + pivot;                                                         \
        d = c + pivot;                                                         \
        pivot = 1;                                                             \
                                                                               \
        if (less(d, c, arg)) {                                                 \
          pivot = 0;                                                           \
          if (less(e, d, arg)) {                                               \
            sort_swap(c, e);                                                   \
          } else {                                                             \
            sort_swap(c, d);                                                   \
          }                                                                    \
        } else if (less(e, c, arg)) {                                          \
          pivot = 0;                                                           \
          sort_swap(c, e);                                                     \
        }                                                                      \
        if (less(c, a, arg)) {                                                 \
          pivot = 0;                                                           \
          sort_swap(a, c);                                                     \
          if (less(d, c, arg)) {                                               \
            if (less(e, d, arg)) {                                             \
              sort_swap(c, e);                                                 \
            } else {                                                           \
              sort_swap(c, d);                                                 \
            }                                                                  \
          } else if (less(e, c, arg)) {                                        \
            sort_swap(c, e);                                                   \
          }                                                                    \
        }                                                                      \
        if (less(c, b, arg)) {                                                 \
          pivot = 0;                                                           \
          sort_swap(b, c);                                                     \
          if (less(d, c, arg)) {                                               \
            if (less(e, d, arg)) {                                             \
              sort_swap(c, e);                                                 \
            } else {                                                           \
              sort_swap(c, d);                                                 \
            }                                                                  \
          } else if (less(e, c, arg)) {                                        \
            sort_swap(c, e);                                                   \
          }                                                                    \
        }                                                                      \
                                                                               \
        sort_swap(b, a + 1);                                                   \
        sort_swap(d, e - 1);                                                   \
        b = a + 2;                                                             \
        d = e - 2;                                                             \
        e = c;                                                                 \
        while (1) {                                                            \
          while (less(b, c, arg))                                              \
            b++;                                                               \
          while (less(c, d, arg))                                              \
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
          if (pivot == 0 && !(less(base, base + 1, arg))) {                    \
            a = base + 2;                                                      \
            while (a <= high && !(less(base, a, arg)))                         \
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
