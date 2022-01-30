#include <stdio.h>

void clear_screen() {
    printf( "\x1B[2J" );
}

int main( int argc, char *argv[] ) {
    clear_screen();
    return 0;
}