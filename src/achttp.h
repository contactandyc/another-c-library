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

#ifndef achttp_H
#define achttp_H
#include <stddef.h>
#include <stdint.h>

typedef struct achttp_group_s achttp_group_t;
typedef struct achttp_s achttp_t;

typedef void (*achttp_f)(achttp_t *);
typedef void (*achttp_data_f)(achttp_t *, char const *data,
                                 size_t data_length);

/*  Init/destroy group container for parsers  */
achttp_group_t *achttp_group_init(achttp_f headers,
                                        achttp_data_f chunk,
                                        achttp_data_f end,
                                        achttp_f error);
void achttp_group_destroy(achttp_group_t *);

/*  Initialize/release an http parser  */
achttp_t *achttp_init(achttp_group_t *);
void achttp_release(achttp_t *);

/*  Setter getter for user defined data  */
void achttp_set_arg(achttp_t *, void *);
void *achttp_get_arg(achttp_t *);

/*  Add data to the http parser.  Registered callbacks will be called
    for each parsing event.  */
void achttp_parse(achttp_t *, char const *data, size_t data_length);

/*  Get method of request  */
char const *achttp_method(achttp_t *);

/*  Get uri of request  */
char const *achttp_uri(achttp_t *);

/*  Parameter locations  */
typedef enum { header, query, body } achttp_param_location_t;

/*  Getters for parameters  */
char const *achttp_param(achttp_t *, achttp_param_location_t,
                            char const *key, char const *default_value /*=""*/);

/*  Get multiple params for a single key.  Returned value array is
    NULL terminated and valid for the life of the request  */
char **achttp_params(achttp_t *, achttp_param_location_t,
                        char const *key);

#endif
