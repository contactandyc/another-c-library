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

#include "stla_buffer.h"

int main( int argc, char *argv[] ) {
  stla_buffer_t *bh = stla_buffer_init(10);
  stla_buffer_sets(bh, "Hello");
  stla_buffer_append(bh, " Buffer!", strlen(" Buffer!")); // append
  /* print Hello Buffer! followed with a newline */
  printf( "%s\n", stla_buffer_data(bh) );
  stla_buffer_destroy(bh);
  return 0;
}
