ROOT=../../..
OBJECTS=$(ROOT)/src/ac_timer.c
HEADER_FILES=$(ROOT)/src/ac_timer.h
FLAGS=-O3 -I$(ROOT)/src

all: test_timer examples

test_timer: test_timer.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) test_timer.c -o test_timer

examples:
	./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse

clean:
	rm -f test_timer *~
