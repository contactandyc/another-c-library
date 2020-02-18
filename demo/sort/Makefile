ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -O3 -lz -luv

all: qsort_test ac_sort_test ac_sort_test_compare sort_test sort_test_compare demo

qsort_test: qsort_test.c $(OBJECTS)
	gcc $(FLAGS) $(OBJECTS) qsort_test.c -o qsort_test

ac_sort_test: ac_sort_test.c $(OBJECTS)
	gcc $(FLAGS) $(OBJECTS) ac_sort_test.c -o ac_sort_test

ac_sort_test_compare: ac_sort_test_compare.c $(OBJECTS)
	gcc $(FLAGS) $(OBJECTS) ac_sort_test_compare.c -o ac_sort_test_compare

sort_test: sort_test.cc $(OBJECTS)
	c++ $(FLAGS) $(OBJECTS) sort_test.cc -o sort_test

sort_test_compare: sort_test_compare.cc $(OBJECTS)
	c++ $(FLAGS) $(OBJECTS) sort_test_compare.cc -o sort_test_compare

sort_compare.out: sort_test_compare
	./sort_test_compare > sort_compare.out

sort.out: sort_test
	./sort_test > sort.out

qsort.out: qsort_test
	./qsort_test > qsort.out

ac_sort.out: ac_sort_test
	./ac_sort_test > ac_sort.out

ac_sort_compare.out: ac_sort_test_compare
	./ac_sort_test_compare > ac_sort_compare.out

demo: ../tools/side_by_side sort.out sort_compare.out qsort.out ac_sort.out ac_sort_compare.out
	@echo std::sort vs std::sort_compare
	@../tools/side_by_side sort.out sort_compare.out
	@echo ac_sort vs std::sort
	@../tools/side_by_side ac_sort.out sort.out
	@echo ac_sort_compare vs std::sort
	@../tools/side_by_side ac_sort_compare.out sort.out
	@echo ac_sort_compare vs std::sort_compare
	@../tools/side_by_side ac_sort_compare.out sort_compare.out
	@echo ac_sort vs qsort
	@../tools/side_by_side ac_sort.out qsort.out
	@echo ac_sort_compare vs qsort
	@../tools/side_by_side ac_sort_compare.out qsort.out
	@echo ac_sort vs ac_sort_compare
	@../tools/side_by_side ac_sort.out ac_sort_compare.out

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
