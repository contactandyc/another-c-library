ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -g -D_AC_DEBUG_MEMORY_=NULL -lz
PROGRAMS = start order_tasks

all: $(PROGRAMS) examples gitignore

$(PROGRAMS): %: %.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) $< -o $@

examples: $(PROGRAMS)
	@echo Running the examples
	@echo
	rm -rf tasks
	./start
	@echo
	rm -rf tasks
	./order_tasks

clean:
	rm -rf *~ *.dSYM $(PROGRAMS)

gitignore:
	@echo "*~" > .gitignore
	@echo "*.dSYM" >> .gitignore
	@for prog in $(PROGRAMS) ; do \
		echo $$prog >> .gitignore; \
	done
