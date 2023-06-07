#include "ac_boolean_tree_node.h"

static
ac_boolean_tree_node_t * _ac_boolean_tree_node_parse(ac_pool_t *pool, char **s, char *ep, ac_boolean_tree_node_t *parent) {
    char *p = *s;
    char *term = p;
    ac_boolean_tree_node_t *head = ac_pool_calloc(pool, sizeof(*head));
    ac_boolean_tree_node_t *tail = head;
    head->term = p;
    head->parent = parent;
    while(p < ep) {
        if((*p == '|' && p[1] == '|') ||
           (*p == '&' && p[1] == '&') ||
           (*p == '-' && p[1] == '-')) {
            ac_boolean_tree_node_t *n = ac_pool_calloc(pool, sizeof(*n));
            n->op = *p;
            *p = 0;
            p += 2;
            n->term = p;
            n->parent = parent;
            n->prev = tail;
            tail->next = n;
            tail = n;
        }
        else if(*p == '[') {
            p++;
            tail->child = _ac_boolean_tree_node_parse(pool, &p, ep, tail);
            if(!tail->child)
                break;
            tail->term = NULL;
        }
        else if(*p == ']') {
            *p = 0;
            p++;
            break;
        }
        else
            p++;
    }
    *s = p;
    return head;
}

ac_boolean_tree_node_t * ac_boolean_tree_node_parse(ac_pool_t *pool, const char *str) {
    char *s = ac_pool_strdup(pool, str);
    char *ep = s + strlen(s);
    return _ac_boolean_tree_node_parse(pool, &s, ep, NULL);
}

void ac_boolean_tree_node_print(ac_boolean_tree_node_t *n, int indent) {
    while(n) {
        if(n->term) {
            for( int i=0; i<indent; i++ )
                printf( "....");
            printf( "%s %c\n", n->term, n->op ? n->op : ' ');
        }
        else
            ac_boolean_tree_node_print(n->child, indent+1);
        n = n->next;
    }
}
