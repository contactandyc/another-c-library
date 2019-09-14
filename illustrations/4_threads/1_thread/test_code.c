#include <stdio.h>
#include <pthread.h>

int global_number;

void *worker(void *arg) {
  for( int i=0; i<1000000; i++ )
    global_number++;
  return NULL;
}

int main(int argc, char *argv[]) {
  global_number = 0;
  pthread_t threads[10];
  for( int i=0; i<10; i++ )
    pthread_create(&threads[i], NULL, worker, NULL);

  for( int i=0; i<10; i++ )
    pthread_join(threads[i], NULL);
  printf( "global_number (should be 10000000)= %d\n", global_number );
  return 0;
}
