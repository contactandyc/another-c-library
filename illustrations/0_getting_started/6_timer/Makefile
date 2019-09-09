all: test_timer examples

test_timer: test_timer.c file2.c
	gcc -O3 test_timer.c -o test_timer

examples:
	./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse

clean:
	rm -f test_timer test_reverse *~
