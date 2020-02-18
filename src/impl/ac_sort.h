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

#include "ac_sort/ac_sort_aux.h"

#define sort_swap(a, b)                                                        \
  tmp = *(a);                                                                  \
  *(a) = *(b);                                                                 \
  *(b) = tmp

typedef struct {
  void *base;
  ssize_t num_elements;
} sort_stack_t;

#include "ac_sort/ac_sort_arg_m.h"
#include "ac_sort/ac_sort_compare_arg_m.h"
#include "ac_sort/ac_sort_compare_m.h"
#include "ac_sort/ac_sort_m.h"
#include "ac_sort/ac_sort_type_m.h"
