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

#ifndef _acsearch_H
#define _acsearch_H

/*
  Search macros
  =====================================================================

  acsearch_m(name, keytype, datatype, compare)
  acsearch_least_m(name, keytype, datatype, compare)
  acsearch_greatest_m(name, keytype, datatype, compare)
  acsearch_lower_bound_m(name, keytype, datatype, compare)
  acsearch_upper_bound_m(name, keytype, datatype, compare)

    expects: int compare(const keytype *k, const datatype *v);
    returns: datatype *name(keytype *k, datatype *base, size_t num_elements);


  acsearch_arg_m(name, keytype, datatype, compare)
  acsearch_least_arg_m(name, keytype, datatype, compare)
  acsearch_greatest_arg_m(name, keytype, datatype, compare)
  acsearch_lower_bound_arg_m(name, keytype, datatype, compare)
  acsearch_upper_bound_arg_m(name, keytype, datatype, compare)

    expects: int compare(const keytype *k, const datatype *v, void *arg);
    returns: datatype *name(keytype *k, datatype *base,
                            size_t num_elements, void *arg);

  acsearch_compare_m(name, keytype, datatype, compare)
  acsearch_least_compare_m(name, keytype, datatype, compare)
  acsearch_greatest_compare_m(name, keytype, datatype, compare)
  acsearch_lower_bound_compare_m(name, keytype, datatype, compare)
  acsearch_upper_bound_compare_m(name, keytype, datatype, compare)

    returns: datatype *name(keytype *k,
                            datatype *base,
                            size_t num_elements,
                            int compare(const keytype *k,
                                        const datatype *v));


  acsearch_compare_arg_m(name, keytype, datatype, compare)
  acsearch_least_compare_arg_m(name, keytype, datatype, compare)
  acsearch_greatest_compare_arg_m(name, keytype, datatype, compare)
  acsearch_lower_bound_compare_arg_m(name, keytype, datatype, compare)
  acsearch_upper_bound_compare_arg_m(name, keytype, datatype, compare)

    returns: datatype *name(keytype *k,
                            datatype *base,
                            size_t num_elements,
                            int compare(const keytype *k,
                                        const datatype *v,
                                        void *arg),
                            void *arg);
*/

#include "impl/acsearch.h"

#endif
