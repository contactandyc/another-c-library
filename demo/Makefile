ROOT=..
include $(ROOT)/src/Makefile.include

FLAGS += -D_AC_DEBUG_MEMORY_=NULL
PROGRAMS=quicksort_demo demo1 demo2 demo3 demo4 demo5

all: $(PROGRAMS) examples

$(PROGRAMS): %: %.c
	gcc $(FLAGS) $(OBJECTS) $< -o $@

examples:
	./quicksort_demo ABCDEFGHIJKLMNOPQRSTUVWXYZ
	@echo
	./demo1
	@echo
	./demo2
	@echo
	./demo3
	@echo
	./demo4
	@echo
	./demo5 "Ada Verdun Howell" "B"
	@echo
	./quicksort_demo A 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
	@echo
	./quicksort_demo A 1000

clean:
	rm -rf *~ *.dSYM demo1 demo2 demo3 demo4 demo5 quicksort_demo
