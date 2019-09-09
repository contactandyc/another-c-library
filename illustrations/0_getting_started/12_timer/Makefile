ROOT=../../..
OBJECTS=$(ROOT)/src/stla_timer.c
FLAGS=-I$(ROOT)/src

all: test_timer examples

test_timer: test_timer.c
	gcc $(FLAGS) $(OBJECTS) test_timer.c -o test_timer

examples:
	@./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse

clean:
	rm -f test_timer *~
