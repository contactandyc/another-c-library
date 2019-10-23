all: test_timer examples

test_timer: test_timer.c ac_timer.c ac_timer.h
	gcc -O3 ac_timer.c test_timer.c -o test_timer

examples:
	./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse

clean:
	rm -f test_timer *~
