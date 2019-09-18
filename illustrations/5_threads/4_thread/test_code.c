#include <stdio.h>
#include <pthread.h>

typedef struct {
  int local_number;
  pthread_mutex_t mutex;
} worker_t;

void *worker(void *arg) {
  worker_t *w = (worker_t *)arg;
  for( int i=0; i<1000000; i++ ) {
    pthread_mutex_lock(&(w->mutex));
    w->local_number++;
    pthread_mutex_unlock(&(w->mutex));
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_t threads[10];
  worker_t w;
  w.local_number = 0;
  pthread_mutex_init(&w.mutex, NULL);
  for( int i=0; i<10; i++ )
    pthread_create(&threads[i], NULL, worker, &w);

  for( int i=0; i<10; i++ )
    pthread_join(threads[i], NULL);
  pthread_mutex_destroy(&w.mutex);
  printf( "local_number (should be 10000000)= %d\n", w.local_number );
  return 0;
}
