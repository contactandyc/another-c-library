ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -D_AC_DEBUG_MEMORY_=NULL -lz
PROGRAMS=ac_malloc ac_calloc

all: $(PROGRAMS) examples

$(PROGRAMS): %: %.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) $< -o $@

examples: $(PROGRAMS)
	@echo Running the examples
	./ac_malloc
	@echo
	./ac_calloc
	@echo

clean:
	rm -rf *~ *.dSYM $(PROGRAMS)
