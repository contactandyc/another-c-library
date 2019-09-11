#ifndef _xxx_H
#define _xxx_H

struct xxx_s;
typedef struct xxx_s xxx_t;

xxx_t * xxx_init(int param1, int param2);
void xxx_destroy( xxx_t *h );

void xxx_do_something( xxx_t *h, const char *prefix );

#endif
