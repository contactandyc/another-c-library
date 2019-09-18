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

void print_value_of_pointer(char type, void *p) {
  if(type == 'c') {
    char *vp = (char *)p;
    printf( "char type: %d (%c)\n", *vp, *vp );
  }
  else if(type == 'C') {
    unsigned char *vp = (unsigned char *)p;
    printf( "unsigned char type: %u\n", *vp );
  }
  else if(type == 's') {
    short *vp = (short *)p;
    printf( "short type: %d\n", *vp );
  }
  else if(type == 'S') {
    unsigned short *vp = (unsigned short *)p;
    printf( "unsigned short type: %u\n", *vp );
  }
  else if(type == 'i') {
    int *vp = (int *)p;
    printf( "int type: %d\n", *vp);
  }
  else if(type == 'I') {
    unsigned int *vp = (unsigned int *)p;
    printf( "unsigned int type: %u\n", *vp );
  }
  else if(type == 'l') {
    long *vp = (long *)p;
    printf( "long type: %ld\n", *vp);
  }
  else if(type == 'L') {
    unsigned long *vp = (unsigned long *)p;
    printf( "unsigned long type: %lu\n", *vp );
  }
  else if(type == 'd') {
    double *vp = (double *)p;
    printf( "double type (with 4 decimal places): %0.4f\n", *vp );
  }
}

int main( int argc, char *argv[]) {
  char a = '0';
  print_value_of_pointer('c', &a); // notice that printable 0 is actually 48
  unsigned char b = 150;
  print_value_of_pointer('C', &b);
  short c = -5000;
  print_value_of_pointer('s', &c);
  unsigned short d = 5000;
  print_value_of_pointer('S', &d);
  int e = -500000;
  print_value_of_pointer('i', &e);
  unsigned int f = 500000;
  print_value_of_pointer('I', &f);
  long g = -500000;
  print_value_of_pointer('l', &g);
  unsigned long h = 500000;
  print_value_of_pointer('L', &h);
  double i = 1.5;
  print_value_of_pointer('d', &i);
  return 0;
}
