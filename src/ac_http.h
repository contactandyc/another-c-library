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

#ifndef ac_http_H
#define ac_http_H
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ac_http_group_s ac_http_group_t;
typedef struct ac_http_s ac_http_t;

typedef void (*ac_http_f)(ac_http_t *);
typedef void (*ac_http_data_f)(ac_http_t *, char const *data,
                               size_t data_length);

/*  Init/destroy group container for parsers  */
ac_http_group_t *ac_http_group_init(ac_http_f headers, ac_http_data_f chunk,
                                    ac_http_data_f end, ac_http_f error);
void ac_http_group_destroy(ac_http_group_t *);

/*  Initialize/release an http parser  */
ac_http_t *ac_http_init(ac_http_group_t *);
void ac_http_release(ac_http_t *);

/*  Setter getter for user defined data  */
void ac_http_set_arg(ac_http_t *, void *);
void *ac_http_get_arg(ac_http_t *);

/*  Add data to the http parser.  Registered callbacks will be called
    for each parsing event.  */
void ac_http_parse(ac_http_t *, char const *data, size_t data_length);

/*  Get method of request  */
char const *ac_http_method(ac_http_t *);

/*  Get uri of request  */
char const *ac_http_uri(ac_http_t *);

/*  Parameter locations  */
typedef enum { header, query, body } ac_http_param_location_t;

/*  Getters for parameters  */
char const *ac_http_param(ac_http_t *, ac_http_param_location_t,
                          char const *key, char const *default_value /*=""*/);

/*  Get multiple params for a single key.  Returned value array is
    NULL terminated and valid for the life of the request  */
char **ac_http_params(ac_http_t *, ac_http_param_location_t, char const *key);

#ifdef __cplusplus
}
#endif

#endif
