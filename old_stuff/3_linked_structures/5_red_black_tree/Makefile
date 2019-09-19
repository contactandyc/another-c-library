ROOT=../../..
include $(ROOT)/src/Makefile.include

FLAGS += -D_STLA_DEBUG_MEMORY_=NULL
DATA_STRUCTURE=red_black_tree

all: test_data_structure examples

test_data_structure: test_data_structure.c data_structure.h $(DATA_STRUCTURE).c $(HEADER_FILES) $(OBJECTS)
	gcc -g $(FLAGS) $(OBJECTS) test_data_structure.c $(DATA_STRUCTURE).c -o test_data_structure -DDATA_STRUCTURE=\"$(DATA_STRUCTURE)\"

examples:
	@./test_data_structure PDCBAEMLOQTRYZ

clean:
	rm -f test_data_structure *~
