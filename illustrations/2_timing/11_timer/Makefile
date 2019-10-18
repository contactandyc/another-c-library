all: test_timer examples

test_timer: test_timer.c actimer.c actimer.h
	gcc -O3 actimer.c test_timer.c -o test_timer

examples:
	./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse

clean:
	rm -f test_timer *~
