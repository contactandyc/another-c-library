ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -D_AC_DEBUG_MEMORY_=NULL -lz
PROGRAMS = ac_json_parse

all: $(PROGRAMS) examples gitignore

$(PROGRAMS): %: %.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) $< -o $@

examples: $(PROGRAMS)
	@echo Running the examples
	@echo
	./ac_json_parse

clean:
	rm -rf *~ *.dSYM $(PROGRAMS)

gitignore:
	@echo "*~" > .gitignore
	@echo "*.dSYM" >> .gitignore
	@for prog in $(PROGRAMS) ; do \
		echo $$prog >> .gitignore; \
	done
