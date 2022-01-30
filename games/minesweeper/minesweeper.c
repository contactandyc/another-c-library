#include <stdio.h>
#include <stdlib.h>

void clear_screen() {
    printf( "\e[1;1H\e[2J" );
}

void print_token(int **board, int size, int i, int j ) {
    if(board[i][j] == -1) {
        printf(" ? ");
    }
}

void print_minesweeper(int **board, int size) { 
  printf( "\n        " );
  for( int i=0; i<size; i++ ) {
    printf (" %c ", 'A'+i );
  }
  printf ("\n\n");
  for( int i=0; i<size; i++ ) {
      printf ("    %d   ", i );
      for( int j=0; j<size; j++ ) {
          print_token(board, size, i, j);
      }
      printf ("\n");
  }  
}

int main( int argc, char *argv[] ) {
    clear_screen();
    int size = 10;
    int **board = (int **)malloc(sizeof(int *)*size);
    for( int i=0; i<size; i++ ) {
        board[i] = (int *)malloc(sizeof(int)*size);
        for( int j=0; j<size; j++ ) {
            board[i][j] = -1;
        }
    }
    print_minesweeper(board, size);
    return 0;
}