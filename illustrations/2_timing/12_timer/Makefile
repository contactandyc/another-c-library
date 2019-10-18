ROOT=../../..
OBJECTS=$(ROOT)/src/actimer.c
HEADER_FILES=$(ROOT)/src/actimer.h
FLAGS=-O3 -I$(ROOT)/src

all: test_timer examples

test_timer: test_timer.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) test_timer.c -o test_timer

examples:
	./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse

clean:
	rm -f test_timer *~
