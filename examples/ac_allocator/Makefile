ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -D_AC_DEBUG_MEMORY_=NULL -lz
PROGRAMS=ac_malloc ac_calloc ac_realloc ac_memdup ac_strdup ac_strdupf ac_strdupvf ac_strdupa ac_strdupa2 ac_strdupan

all: $(PROGRAMS) examples gitignore

$(PROGRAMS): %: %.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) $< -o $@

examples: $(PROGRAMS)
	@echo Running the examples
	./ac_malloc
	@echo
	./ac_calloc
	@echo
	./ac_realloc
	@echo
	./ac_memdup
	@echo
	./ac_strdup this is a test
	@echo
	./ac_strdupf
	@echo
	./ac_strdupvf
	@echo
	./ac_strdupa
	@echo
	./ac_strdupa2
	@echo
	./ac_strdupan this is a test
	@echo

clean:
	rm -rf *~ *.dSYM $(PROGRAMS)

gitignore:
	@echo "*~" > .gitignore
	@echo "*.dSYM" >> .gitignore
	@for prog in $(PROGRAMS) ; do \
		echo $$prog >> .gitignore; \
	done
