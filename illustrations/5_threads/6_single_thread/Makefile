all: test_code

test_code: test_code.c
	gcc -O3 test_code.c -o test_code -lpthread

examples:
	./test_code

clean:
	rm -f test_code *~
