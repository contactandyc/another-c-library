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
	./test_data_structure AB 0
	./test_data_structure CAB 0
	./test_data_structure ABCDEF 0
	./test_data_structure AFCEDB 0

clean:
	rm -f test_data_structure *~
