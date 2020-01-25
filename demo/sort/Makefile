ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -O3 -lz

all: qsort_test ac_sort_test demo

qsort_test: qsort_test.c $(OBJECTS)
	gcc $(FLAGS) $(OBJECTS) qsort_test.c -o qsort_test

ac_sort_test: ac_sort_test.c $(OBJECTS)
	gcc $(FLAGS) $(OBJECTS) ac_sort_test.c -o ac_sort_test

demo: ../tools/side_by_side
	@./qsort_test > qsort.out
	@./ac_sort_test > ac_sort.out
	@../tools/side_by_side ac_sort.out qsort.out

web:
	@../tools/side_by_side ac_sort.out qsort.out | ../tools/webify.sh

../tools/side_by_side: ../tools/side_by_side.c
	cd ../tools && make && cd -

clean:
	rm -rf *~ *.dSYM qsort_test ac_sort_test *.out
