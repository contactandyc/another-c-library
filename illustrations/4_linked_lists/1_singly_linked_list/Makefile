DATA_STRUCTURE=singly_linked_list
ROOT=../../..
include $(ROOT)/src/Makefile.include
FLAGS += -DDATA_STRUCTURE=\"$(DATA_STRUCTURE)\"
OBJECTS += $(DATA_STRUCTURE).c
HEADER_FILES += data_structure.h

all: test_data_structure examples

test_data_structure: test_data_structure.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) test_data_structure.c -o test_data_structure

examples:
	@./test_data_structure AB CAB ABCDEF AFCEDB

clean:
	rm -f test_data_structure *~
