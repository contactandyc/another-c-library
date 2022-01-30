#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void clear_screen() {
    printf( "\e[1;1H\e[2J" );
}

#define RED_START "\x1B[31m"
#define RED_END "\x1B[0m"

void print_token(int **board, int size, int i, int j ) {
    if(board[i][j] == 0) {
        printf(" ? ");
    }
    else if(board[i][j] == -1) {
        printf(RED_START " * " RED_END );
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

void place_bombs( int **board, int size, int num_bombs ) {
    for( int i=0; i<num_bombs; i++ ) {
        int x,y;
        do {
          x = rand() % size;
          y = rand() % size;
        } while(board[x][y] != 0);
        board[x][y] = -1;
    }
}

int main( int argc, char *argv[] ) {
    srand(time(NULL));
    clear_screen();
    int size = 10;
    int **board = (int **)malloc(sizeof(int *)*size);
    for( int i=0; i<size; i++ ) {
        board[i] = (int *)malloc(sizeof(int)*size);
        for( int j=0; j<size; j++ ) {
            board[i][j] = 0;
        }
    }
    place_bombs(board, size, 5);
    print_minesweeper(board, size);
    return 0;
}