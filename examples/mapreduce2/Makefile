ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -g -D_AC_DEBUG_MEMORY_=NULL -lz
PROGRAMS = start order_tasks
PROGRAMS += input_data_1 input_data_2 input_data_3 input_data_4
PROGRAMS += data_pipeline_1 data_pipeline_2

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
	@echo
	rm -rf tasks
	./input_data_1
	@echo
	rm -rf tasks
	./input_data_2 --dir sample --ext txt,tbontb
	./input_data_2 -s -t split
	@echo
	rm -rf tasks
	./input_data_3 --dir .. --ext c
	./input_data_3 -s -t split
	@echo
	rm -rf tasks
	./data_pipeline_1 --dir . --ext c
	./data_pipeline_1 -s -t split first multi
	@echo
	./data_pipeline_2 -s -t multi
clean:
	rm -rf *~ *.dSYM $(PROGRAMS)

gitignore:
	@echo "*~" > .gitignore
	@echo "*.dSYM" >> .gitignore
	@for prog in $(PROGRAMS) ; do \
		echo $$prog >> .gitignore; \
	done
