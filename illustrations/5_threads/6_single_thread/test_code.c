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

int main(int argc, char *argv[]) {
  int local_number = 0;
  for( int j=0; j<10; j++ ) {
    for( int i=0; i<1000000; i++ )
      local_number++;
  }
  printf( "local_number (should be 10000000)= %d\n", local_number );
  return 0;
}
