ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -O3 -lz

all: qsort_test ac_sort_test sort_test demo

qsort_test: qsort_test.c $(OBJECTS)
	gcc $(FLAGS) $(OBJECTS) qsort_test.c -o qsort_test

ac_sort_test: ac_sort_test.c $(OBJECTS)
	gcc $(FLAGS) $(OBJECTS) ac_sort_test.c -o ac_sort_test

sort_test: sort_test.cc $(OBJECTS)
	c++ $(FLAGS) $(OBJECTS) sort_test.cc -o sort_test

demo: ../tools/side_by_side
	@./sort_test > sort.out
	@./ac_sort_test > ac_sort.out
	@./qsort_test > qsort.out
	@echo ac_sort vs std::sort
	@../tools/side_by_side ac_sort.out sort.out
	@echo ac_sort vs qsort
	@../tools/side_by_side ac_sort.out qsort.out

term:
	@echo ac_sort vs std::sort
	@../tools/side_by_side ac_sort.out sort.out
	@echo ac_sort vs qsort
	@../tools/side_by_side ac_sort.out qsort.out


web:
	@echo ac_sort vs std::sort | ../tools/webify.sh
	@../tools/side_by_side ac_sort.out sort.out | ../tools/webify.sh
	@echo ac_sort vs qsort | ../tools/webify.sh
	@../tools/side_by_side ac_sort.out qsort.out | ../tools/webify.sh

../tools/side_by_side: ../tools/side_by_side.c
	cd ../tools && make && cd -

clean:
	rm -rf *~ *.dSYM sort_test qsort_test ac_sort_test *.out
