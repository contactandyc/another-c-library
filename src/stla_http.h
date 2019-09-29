#ifndef stla_http_H
#define stla_http_H
#include <stddef.h>
#include <stdint.h>

typedef struct stla_http_group_s stla_http_group_t;
typedef struct stla_http_s stla_http_t;

typedef void (*stla_http_f)(stla_http_t *);
typedef void (*stla_http_data_f)(stla_http_t *, char const *data,
                                 size_t data_length);

/*  Init/destroy group container for parsers  */
stla_http_group_t *stla_http_group_init(stla_http_f headers,
                                        stla_http_data_f chunk,
                                        stla_http_data_f end,
                                        stla_http_f error);
void stla_http_group_destroy(stla_http_group_t *);

/*  Initialize/release an http parser  */
stla_http_t *stla_http_init(stla_http_group_t *);
void stla_http_release(stla_http_t *);

/*  Setter getter for user defined data  */
void stla_http_set_arg(stla_http_t *, void *);
void *stla_http_get_arg(stla_http_t *);

/*  Add data to the http parser.  Registered callbacks will be called
    for each parsing event.  */
void stla_http_parse(stla_http_t *, char const *data, size_t data_length);

/*  Get method of request  */
char const *stla_http_method(stla_http_t *);

/*  Get uri of request  */
char const *stla_http_uri(stla_http_t *);

/*  Parameter locations  */
typedef enum { header, query, body } stla_http_param_location_t;

/*  Getters for parameters  */
char const *stla_http_param(stla_http_t *, stla_http_param_location_t,
                            char const *key, char const *default_value /*=""*/);

/*  Get multiple params for a single key.  Returned value array is
    NULL terminated and valid for the life of the request  */
char **stla_http_params(stla_http_t *, stla_http_param_location_t,
                        char const *key);

#endif
