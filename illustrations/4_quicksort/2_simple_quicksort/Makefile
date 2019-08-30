all: simple_quicksort examples

simple_quicksort: simple_quicksort.c timer.c
	gcc timer.c simple_quicksort.c -o simple_quicksort

examples:
	./simple_quicksort this is a simple version of the quicksort algorithm ABCDEFGHIJKLMNOPQRSTUVWXYZ AAAAAAAAAAAAAAAAAAAAAAAAAA

clean:
	rm -f simple_quicksort *~
