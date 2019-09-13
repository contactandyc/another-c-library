DATA_STRUCTURE=binary_search_tree

all: test_data_structure examples

test_data_structure: test_data_structure.c buffer.h buffer.c data_structure.h $(DATA_STRUCTURE).c
	gcc test_data_structure.c buffer.c $(DATA_STRUCTURE).c -o test_data_structure -DDATA_STRUCTURE=\"$(DATA_STRUCTURE)\"

examples:
	@./test_data_structure PDCBAEMLOQ
	# PDCBAEMLOQTRYZ

clean:
	rm -f test_data_structure *~
