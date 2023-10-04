#ifndef _ac_token_h
#define _ac_token_h

#include "another-c-library/ac_pool.h"

struct ac_token_s;
typedef struct ac_token_s ac_token_t;

typedef enum { 
    AC_TOKEN_TOKEN=0, 
    AC_TOKEN_NUMBER=10, 
    AC_TOKEN_OPERATOR=20, 
    AC_TOKEN_COMPARISON=30,
    AC_TOKEN_MODIFIER=40, 
    AC_TOKEN_AND=50, 
    AC_TOKEN_OR=60, 
    AC_TOKEN_NOT=65,
    AC_TOKEN_DASH=70, 
    AC_TOKEN_OPEN_PAREN=90, 
    AC_TOKEN_OPEN_BRACE=91, 
    AC_TOKEN_OPEN_BRACKET=92, 
    AC_TOKEN_QUOTE=93, 
    AC_TOKEN_DQUOTE=94, 
    AC_TOKEN_CLOSE_PAREN=100, 
    AC_TOKEN_CLOSE_BRACE=101, 
    AC_TOKEN_CLOSE_BRACKET=102, 
    AC_TOKEN_NULL=110,
    AC_TOKEN_COLON=111,
    AC_TOKEN_QUESTION=112,
    AC_TOKEN_COMMA=130,
    AC_TOKEN_SPACE=140, 
    AC_TOKEN_OTHER=150 } ac_token_type_t;

typedef enum {
    NORMAL=0, SKIP=1, NEXT=2, GLOBAL=3, NUMBER=4
} ac_token_cb_t;

struct ac_token_s {
    ac_token_type_t type;
    char *token;

    ac_token_cb_t attr_type;
    bool no_params;

    char **attrs;
    uint32_t num_attrs;

    ac_token_t *attr;

    ac_token_t *next;
    ac_token_t *prev;
    ac_token_t *child;
    ac_token_t *parent;
};

ac_token_t *ac_token_clone(ac_pool_t *pool, ac_token_t *t);

void ac_token_dump(ac_token_t *t);
ac_token_t *ac_token_parse(ac_pool_t *pool, const char *s);

typedef struct {
    ac_pool_t *pool;
    char *param;
    char **attrs;
    uint32_t num_attrs;

    bool no_params;
    bool strip;
    ac_token_t *alt_token;
} ac_token_cb_data_t;

/* this should return SKIP if a parameter is set */
typedef ac_token_cb_t (*ac_token_set_var_cb)(void *arg, ac_token_cb_data_t *d);

ac_token_t *ac_token_parse_expression(ac_pool_t *pool, const char *s,
                                      ac_token_set_var_cb cb, void *arg);

struct ac_token_dict_s;
typedef struct ac_token_dict_s ac_token_dict_t;
ac_token_dict_t *ac_token_dict_init();
ac_token_dict_t *ac_token_dict_load(const char *filename);
void ac_token_dict_destroy(ac_token_dict_t *h);

/* name: global|next|skip|normal|number [no_params] [strip] string */
bool ac_token_dict_add(ac_token_dict_t *h, const char *config_line);
const char *ac_token_dict_value(ac_token_dict_t *h, const char *param);
char **ac_token_dict_values(ac_token_dict_t *h, uint32_t *num_values, const char *param);

ac_token_cb_t ac_token_dict_cb(void *arg, ac_token_cb_data_t *d);

#endif