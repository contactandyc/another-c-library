all: test_timer examples

test_timer: test_timer.c stla_timer.c stla_timer.h
	gcc -O3 stla_timer.c test_timer.c -o test_timer

examples:
	./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse

clean:
	rm -f test_timer *~
