all: test_timer examples

test_timer: test_timer.c timer.c timer.h
	gcc -O3 timer.c timer.h test_timer.c -o test_timer

examples:
	./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse

clean:
	rm -f test_timer *~
