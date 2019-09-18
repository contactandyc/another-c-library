/*
Copyright 2019 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

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
