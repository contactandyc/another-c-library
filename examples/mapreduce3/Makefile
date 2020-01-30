ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -g -D_AC_DEBUG_MEMORY_=NULL -lz
PROGRAMS = data_pipeline_1

all: $(PROGRAMS) examples gitignore

$(PROGRAMS): %: %.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) $< -o $@

examples: $(PROGRAMS)
	@echo Running the examples
	@echo
	rm -rf tasks
	./data_pipeline_1 --dir ../mapreduce2 --ext c
	./data_pipeline_1 -s -t split first multi

clean:
	rm -rf *~ *.dSYM $(PROGRAMS)

gitignore:
	@echo "*~" > .gitignore
	@echo "*.dSYM" >> .gitignore
	@for prog in $(PROGRAMS) ; do \
		echo $$prog >> .gitignore; \
	done
