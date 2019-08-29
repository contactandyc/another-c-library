all: test_quicksort examples

test_quicksort: test_quicksort.c _quicksort.h
	gcc test_quicksort.c -o test_quicksort -O3

examples:
	./test_quicksort A 100000
	./test_quicksort AP 1 44 9 13 345 2 3

clean:
	rm -f test_quicksort *~
