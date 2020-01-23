ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -D_AC_DEBUG_MEMORY_=NULL -lz
PROGRAMS=ac_malloc ac_calloc ac_strdup ac_strdupa ac_strdupa2 ac_strdupan

all: $(PROGRAMS) examples

$(PROGRAMS): %: %.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) $< -o $@

examples: $(PROGRAMS)
	@echo Running the examples
	./ac_malloc
	@echo
	./ac_calloc
	@echo
	./ac_strdup this is a test
	@echo
	./ac_strdupa
	@echo
	./ac_strdupa2
	@echo
	./ac_strdupan this is a test
	@echo

clean:
	rm -rf *~ *.dSYM $(PROGRAMS)
