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

#ifndef _ac_search_H
#define _ac_search_H

/*
  Search macros
  =====================================================================

  ac_search_m(name, keytype, datatype, compare)
  ac_search_least_m(name, keytype, datatype, compare)
  ac_search_greatest_m(name, keytype, datatype, compare)
  ac_search_lower_bound_m(name, keytype, datatype, compare)
  ac_search_upper_bound_m(name, keytype, datatype, compare)

    expects: int compare(const keytype *k, const datatype *v);
    returns: datatype *name(keytype *k, datatype *base, size_t num_elements);


  ac_search_arg_m(name, keytype, datatype, compare)
  ac_search_least_arg_m(name, keytype, datatype, compare)
  ac_search_greatest_arg_m(name, keytype, datatype, compare)
  ac_search_lower_bound_arg_m(name, keytype, datatype, compare)
  ac_search_upper_bound_arg_m(name, keytype, datatype, compare)

    expects: int compare(const keytype *k, const datatype *v, void *arg);
    returns: datatype *name(keytype *k, datatype *base,
                            size_t num_elements, void *arg);

  ac_search_compare_m(name, keytype, datatype, compare)
  ac_search_least_compare_m(name, keytype, datatype, compare)
  ac_search_greatest_compare_m(name, keytype, datatype, compare)
  ac_search_lower_bound_compare_m(name, keytype, datatype, compare)
  ac_search_upper_bound_compare_m(name, keytype, datatype, compare)

    returns: datatype *name(keytype *k,
                            datatype *base,
                            size_t num_elements,
                            int compare(const keytype *k,
                                        const datatype *v));


  ac_search_compare_arg_m(name, keytype, datatype, compare)
  ac_search_least_compare_arg_m(name, keytype, datatype, compare)
  ac_search_greatest_compare_arg_m(name, keytype, datatype, compare)
  ac_search_lower_bound_compare_arg_m(name, keytype, datatype, compare)
  ac_search_upper_bound_compare_arg_m(name, keytype, datatype, compare)

    returns: datatype *name(keytype *k,
                            datatype *base,
                            size_t num_elements,
                            int compare(const keytype *k,
                                        const datatype *v,
                                        void *arg),
                            void *arg);
*/
#ifdef __cplusplus
extern "C" {
#endif

#include "ac-core/ac_search.h"

#ifdef __cplusplus
}
#endif

#endif
