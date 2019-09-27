ROOT=..
include $(ROOT)/src/Makefile.include

FLAGS += -D_STLA_DEBUG_MEMORY_=NULL
PROGRAMS=demo1 demo2 demo3 demo4 demo5

all: $(PROGRAMS) examples

$(PROGRAMS): %: %.c
	gcc $(FLAGS) $(OBJECTS) $< -o $@

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

clean:
	rm -rf *~ *.dSYM demo1 demo2 demo3 demo4 demo5
