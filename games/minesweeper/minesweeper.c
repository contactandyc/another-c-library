#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

void clear_screen() {
    printf( "\e[1;1H\e[2J" );
}

/*
reg add HKEY_CURRENT_USER\Console /v VirtualTerminalLevel /t REG_DWORD /d 0x00000001 /f
https://www.codeproject.com/Tips/5255355/How-to-Put-Color-on-Windows-Console

*/

#define RED_START "\x1B[31m"
#define RED_END "\x1B[0m"

void print_token(int **board, int size, int i, int j, bool print_bombs ) {
    if(board[i][j] == 0) {
        printf(" ? ");
    }
    else if(board[i][j] == -1) {
        if(print_bombs)
            printf(RED_START " * " RED_END );
        else
            printf(" ? ");
    }
    else if(board[i][j] > 1) {
        printf(" %d ", board[i][j] - 1);
    }
    else if(board[i][j] == 1) {
        printf("   " );
    }

}

void print_minesweeper(int **board, int size, bool print_bombs) { 
  printf( "\n        " );
  for( int i=0; i<size; i++ ) {
    printf (" %c ", 'A'+i );
  }
  printf ("\n\n");
  for( int i=0; i<size; i++ ) {
      printf ("    %d   ", i );
      for( int j=0; j<size; j++ ) {
          print_token(board, size, i, j, print_bombs);
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

typedef enum {INVALID_MOVE, GAME_OVER, GOOD_MOVE} move_t;

int check_for_bomb(int **board, int size, int x, int y) {
    if(x < 0 || x >= size) return 0;
    if(y < 0 || y >= size) return 0;
    if(board[x][y] == -1) return 1;
    return 0;
}

int count_bombs(int **board, int size, int x, int y ) {
    int num_bombs = 
        check_for_bomb(board, size, x-1, y-1) +
        check_for_bomb(board, size, x, y-1 ) +
        check_for_bomb(board, size, x+1, y-1) + 
        check_for_bomb(board, size, x-1, y) +
        check_for_bomb(board, size, x+1, y) + 
        check_for_bomb(board, size, x-1, y+1) +
        check_for_bomb(board, size, x, y+1 ) +
        check_for_bomb(board, size, x+1, y+1);
    return num_bombs;
}


void zero_move( int **board, int size, int x, int y ) {
    if(x < 0 || x >= size) return;
    if(y < 0 || y >= size) return;
    if(board[x][y] != 0) return;
    if(count_bombs(board, size, x, y) > 0)
        return;
    board[x][y] = 1;
    // zero_move(board, size, x-1, y-1);
    zero_move(board, size, x, y-1);
    // zero_move(board, size, x+1, y-1);
    zero_move(board, size, x-1, y);
    zero_move(board, size, x+1, y);
    //zero_move(board, size, x-1, y+1);
    zero_move(board, size, x, y+1);
    //zero_move(board, size, x+1, y+1);
}


move_t try_move(int **board, int size, int x, int y ) {
    if(board[x][y] < 0)
        return GAME_OVER;
    else if(board[x][y] != 0)
        return INVALID_MOVE;
    else {
        // how many bombs are around this?
        int num_bombs = count_bombs(board, size, x, y );
        if(num_bombs == 0)
            zero_move(board, size, x, y );
        else
            board[x][y] = num_bombs+1;
        return GOOD_MOVE;
    }
}

move_t do_move(int **board, int size, char *move) {
    int move_result = INVALID_MOVE;
    if(move[0] >= 'A' && move[0] < 'A' + size && 
       move[1] >= '0' && move[1] < '0' + size) {
      move_result = try_move(board, size,  move[1]-'0', move[0]-'A');
    }
    return move_result;
}

bool get_input(int **board, int size, char *move) {
    printf( "\n   Your Move: " );
    if(!fgets(move, 5, stdin))
        return false;

    if(move[0] >= 'a' && move[0] <='z')
        move[0] = move[0] - 'a' + 'A';
    return true;
}

bool did_i_win(int **board, int size) {
    for( int i=0; i<size; i++ ) {
        for( int j=0; j<size; j++ ) {
            if(board[i][j] == 0)
                return false;
        }
    }
    return true;
}


int main( int argc, char *argv[] ) {
    srand(time(NULL));
    int size = 10;
    int **board = (int **)malloc(sizeof(int *)*size);
    for( int i=0; i<size; i++ ) {
        board[i] = (int *)malloc(sizeof(int)*size);
        for( int j=0; j<size; j++ ) {
            board[i][j] = 0;
        }
    }
    place_bombs(board, size, 5);
    clear_screen();
    print_minesweeper(board, size, false);
    char move[10];
    while(true) {
        if(!get_input(board, size, move))
            break;
        clear_screen();
        move_t m = do_move(board, size, move);
        print_minesweeper(board, size, m==GAME_OVER ? true : false);
        if(m == INVALID_MOVE)
            printf( "%s is an invalid move!\n", move );
        else if(m == GAME_OVER) {
            printf( "There was a bomb at %s -- GAME OVER!\n", move);
            break;
        }
        else if(m == GOOD_MOVE) {
            if(did_i_win(board, size)) {
                printf( "You Won!\n");
                break;
            }
        }
    }
    printf( "Bye!\n");
    return 0;
}