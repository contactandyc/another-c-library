ROOT=..
include $(ROOT)/src/Makefile.include

FLAGS += -D_STLA_DEBUG_MEMORY_=NULL
PROGRAMS=demo1 demo2 demo3 demo4 demo5 demo6

all: $(PROGRAMS) examples

$(PROGRAMS): %: %.c
	gcc $(FLAGS) $(OBJECTS) $< -o $@ -luv

examples:
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
	time ./demo6 names.txt

clean:
	rm -rf *~ *.dSYM demo1 demo2 demo3 demo4 demo5 demo6
