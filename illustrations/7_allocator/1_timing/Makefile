ROOT=../../..
include $(ROOT)/src/Makefile.include

FLAGS += -D_STLA_DEBUG_MEMORY_=\"memory.log\"

all: test_timer examples

test_timer: test_timer.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) test_timer.c -o test_timer

examples:
	./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse

clean:
	rm -f test_timer *~ memory.log*
