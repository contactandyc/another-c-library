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

#include "another-c-library/ac_buffer.h"

int main( int argc, char *argv[] ) {
  ac_buffer_t *bh = ac_buffer_init(10);
  ac_buffer_setc(bh, 'H');
  ac_buffer_appendc(bh, 'e');
  ac_buffer_appendn(bh, 'l', 2); // appendn
  ac_buffer_appendc(bh, 'o');
  ac_buffer_appendc(bh, ' ');
  ac_buffer_appendc(bh, 'B');
  ac_buffer_appendc(bh, 'u');
  ac_buffer_appendn(bh, 'f', 2); // appendn
  ac_buffer_appendc(bh, 'e');
  ac_buffer_appendc(bh, 'r');
  ac_buffer_appendc(bh, '!');
  /* print Hello Buffer! followed with a newline */
  printf( "%s\n", ac_buffer_data(bh) );
  ac_buffer_destroy(bh);
  return 0;
}
