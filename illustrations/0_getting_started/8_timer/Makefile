all: test_timer examples

test_timer: test_timer.c file2.h file2.c
	gcc file2.c test_timer.c -o test_timer

examples:
	./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse

clean:
	rm -f test_timer *~
