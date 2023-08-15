#include "ac_pool.h"
#include <stdio.h>

int main( int argc, char *argv[] ) {
    ac_pool_t *pool = ac_pool_init(1024);
    printf( "%s\n", ac_pool_strdupf(pool, "Hello %s", "World"));
    //ac_pool_destroy(pool);
    return 0;
}