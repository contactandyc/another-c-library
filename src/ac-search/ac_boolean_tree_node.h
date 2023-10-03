#ifndef _ac_boolean_tree_node_h
#define _ac_boolean_tree_node_h

#include "another-c-library/ac_pool.h"

struct ac_boolean_tree_node_s;
typedef struct ac_boolean_tree_node_s ac_boolean_tree_node_t;

struct ac_boolean_tree_node_s {
    char *term;
    char op;
    ac_boolean_tree_node_t *child;
    ac_boolean_tree_node_t *parent;
    ac_boolean_tree_node_t *next;
    ac_boolean_tree_node_t *prev;
    void *arg;
};

ac_boolean_tree_node_t * ac_boolean_tree_node_parse(ac_pool_t *pool, const char *str);
void ac_boolean_tree_node_print(ac_boolean_tree_node_t *n, int indent);

#endif