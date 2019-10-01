ROOT=..
include $(ROOT)/src/Makefile.include

FLAGS += -D_STLA_DEBUG_MEMORY_=NULL
PROGRAMS=uvdemo1

all: $(PROGRAMS) examples

$(PROGRAMS): %: %.c
	gcc $(FLAGS) $(OBJECTS) $(UV_OBJECTS) $< -o $@ -luv

examples:
	time ./uvdemo1 ../demo/names.txt

clean:
	rm -rf *~ *.dSYM uvdemo1
