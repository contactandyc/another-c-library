DATA_STRUCTURE=singly_linked_list

all: test_data_structure examples

test_data_structure: test_data_structure.c data_structure.h $(DATA_STRUCTURE).c
	gcc test_data_structure.c $(DATA_STRUCTURE).c -o test_data_structure -DDATA_STRUCTURE=\"$(DATA_STRUCTURE)\"

examples:
	@./test_data_structure AB CAB ABCDEF AFCEDB

clean:
	rm -f test_data_structure *~
