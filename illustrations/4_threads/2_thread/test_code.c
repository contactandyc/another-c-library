#include <stdio.h>
#include <pthread.h>

void *worker(void *arg) {
  int *local_number = (int *)arg;
  for( int i=0; i<1000000; i++ )
    (*local_number)++;
  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_t threads[10];
  int local_number = 0;
  for( int i=0; i<10; i++ )
    pthread_create(&threads[i], NULL, worker, &local_number);

  for( int i=0; i<10; i++ )
    pthread_join(threads[i], NULL);
  printf( "local_number (should be 10000000)= %d\n", local_number );
  return 0;
}
