all: test_xxx examples

test_xxx: test_xxx.c xxx.c xxx.h
	gcc -O3 xxx.c test_xxx.c -o test_xxx

examples:
	./test_xxx

clean:
	rm -f test_xxx *~
