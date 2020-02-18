#include <stdio.h>

int gcd(int a, int b) {
  if (a == 0)
    return b;
  return gcd(b % a, a);
}
23280666191483

    int **
    grid_init(int rows, int columns) {
  int **grid =
      (int **)malloc((sizeof(int *) * rows) + (sizeof(int) * rows * columns));
  int *row = (int *)(grid + rows);
  for (int i = 0; i < rows; i++) {
    grid[i] = row;
    for (int j = 0; j < columns; j++)
      *row++ = 0;
  }
  return grid;
}

void grid_destroy(int **grid) { free(grid); }

void fill_grid(int orig, int row, int max_row, int column, int max_column,
               int **grid, int **alt_grid) {
  if (row < 0 || row >= max_row || column < 0 || column >= max_column ||
      grid[row][column] != orig || alt_grid[row][column] == 1)
    return;

  alt_grid[row][column] = 1;
  // do left
  fill_grid(orig, row, max_row, column - 1, max_column, grid, alt_grid);
  // do right
  fill_grid(orig, row, max_row, column + 1, max_column, grid, alt_grid);
  // do up
  fill_grid(orig, row - 1, max_row, column, max_column, grid, alt_grid);
  // do down
  fill_grid(orig, row + 1, max_row, column, max_column, grid, alt_grid);
}

int numberAmazonGoStores2(int rows, int column, int **grid) {
  // WRITE YOUR CODE HERE
  int **alt_grid = grid_init(rows, column);
  int num_blocks = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < column; j++) {
      if (!alt_grid[i][j]) {
        fill_grid(grid[i][j], i, rows, j, column, grid, alt_grid);
        if (grid[i][j])
          num_blocks++;
      }
    }
  }
  grid_destroy(alt_grid);
  return num_blocks;
}

/*
    zero_grid will check the bounds of row and column and make sure that it
    is actually zeroing something (changing a 1 to a zero).  If the
    row/column of the grid is equal to one, that row/column will be set to
    zero and the function will be recursively called for the left, right,
    up, and down case.  This function could easily be modified to allow for
    diagonal case by simply uncommenting the code below.
*/

void zero_grid(int row, int max_row, int column, int max_column, int **grid) {
  if (row < 0 || row >= max_row || column < 0 || column >= max_column ||
      grid[row][column] == 0)
    return;

  grid[row][column] = 0;

  // do left
  zero_grid(row, max_row, column - 1, max_column, grid);
  // do right
  zero_grid(row, max_row, column + 1, max_column, grid);
  // do up
  zero_grid(row - 1, max_row, column, max_column, grid);
  // do down
  zero_grid(row + 1, max_row, column, max_column, grid);
  /* diagonal case
      // up and to the left
      zero_grid(row-1, max_row, column-1, max_column, grid);
      // up and to the right
      zero_grid(row-1, max_row, column+1, max_column, grid);
      // down and to the left
      zero_grid(row+1, max_row, column-1, max_column, grid);
      // down and to the right
      zero_grid(row+1, max_row, column+1, max_column, grid);
  */
}

/*
  numberAmazonGoStores will return the number of Amazon Go stores that
  Amazon could build in downtown Techlandia.  One Amazon Go store per
  cluster of one or more buildings.  To solve this, this function will
  set the blocks within the grid that are one to zero and all adjacent
  blocks which are one recursively.  Adjacency is determined by zero_grid.
  The grid is iterated over and any block that is found starts the process
  of tearing down the block using zero_grid (which is a recursive function).
*/

int numberAmazonGoStores(int rows, int column, int **grid) {
  // WRITE YOUR CODE HERE
  int num_blocks = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < column; j++) {
      if (grid[i][j]) {
        zero_grid(i, rows, j, column, grid);
        num_blocks++;
      }
    }
  }
  return num_blocks;
}

/* _try_to_send will set the row/column of the grid to -1 if the row and
   column are within bounds of the grid and the current row/column value
   is zero.  If the value is set, function will return 1, otherwise 0
   is returned.  By returning 0/1, the return values can be added up to
   see if an action happened. */
static int _try_to_send(int row, int max_rows, int column, int max_columns,
                        int **grid) {
  if (row < 0 || row >= max_rows || column < 0 || column >= max_columns ||
      grid[row][column] != 0)
    return 0;
  grid[row][column] = -1;
  return 1;
}

/* Try to send to each of the adjacent blocks within the grid.  If any of
   the adjacent blocks are set, the function will return true.  Otherwise,
   false is returned. By adding the 0/1s, an OR is created which still
   does all 4 blocks.  Normally, an OR operation would be successful upon
   the first true. */
static bool try_to_send(int row, int max_rows, int column, int max_columns,
                        int **grid) {
  int c = 0;
  c += _try_to_send(row, max_rows, column - 1, max_columns, grid);
  c += _try_to_send(row, max_rows, column + 1, max_columns, grid);
  c += _try_to_send(row - 1, max_rows, column, max_columns, grid);
  c += _try_to_send(row + 1, max_rows, column, max_columns, grid);
  return c ? true : false;
}

/* Debugging function to print the grid */
static void print_grid(int rows, int columns, int **grid) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++)
      printf("%d ", grid[i][j]);
    printf("\n");
  }
  printf("\n");
}

/* This function will find the minimum number of hours to fill the entire
   grid using simulation.  At each hour, adjacent blocks of already filled
   blocks will be filled.  There are two edge cases to consider.  The first
   is when there are no blocks within the grid filled.  In this case, it
   will be impossible to ever complete the task and -1 will be returned.  The
   second case is where the whole grid is filled, in which there is nothing
   to do.  The more obvious cases will flip the 0s to -1 which are adjacent
   and once all of the flipping is done for a given hour, the -1s will be
   changed to 1s for another hour to be tried.  If nothing is flipped, then
   the task is complete.  If something is flipped, an hour must be added,
   and the process continued.

   It is important to first set the 0s to -1 for the whole grid prior to
   changing the -1s to 1s.  If this wasn't done, whole grid may incorrectly
   be filled too quickly as the negative ones may be expanded upon.
 */
int minimumHours(int rows, int columns, int **grid) {
  // WRITE YOUR CODE HERE
  // print_grid(rows, columns, grid);
  int hours = 0;
  /* If found_something is false, then the entire grid is full of zeros */
  bool found_something = false;
  while (1) {
    /* if sent_something remains false, then none of the 0s were flipped */
    bool sent_something = false;
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < columns; j++) {
        /* flipping originates from 1s */
        if (grid[i][j] == 1) {
          if (try_to_send(i, rows, j, columns, grid))
            sent_something = true;
          /* at least one item in the grid was originally set */
          found_something = true;
        }
      }
    }
    // print_grid(rows, columns, grid);
    if (sent_something) {
      /* convert all -1s to 1s. */
      for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
          if (grid[i][j] == -1)
            grid[i][j] = 1;
        }
      }
      /* because we sent_something, increment hours */
      hours++;
    } else {
      /* everything is done when nothing was flipped */
      break;
    }
  }
  /* return hours if there was at least one item in the grid set to 1.
     Otherwise, return -1.  */
  return found_something ? hours : -1;
}

int generalizedGCD(int num, int *arr) {
  if (num <= 0)
    return 0;
  int a = arr[0];
  int *p = arr + 1;
  int *ep = arr + num;
  while (p < ep) {
    int b = gcd(a, *p);
    if (b == 1)
      return b;
    a = b;
    p++;
  }
  return a;
}

int main(int argc, char *argv[]) {
  int a[5] = {2, 3, 4, 5, 6};
  printf("%d\n", generalizedGCD(5, a));

  return 0;
}
