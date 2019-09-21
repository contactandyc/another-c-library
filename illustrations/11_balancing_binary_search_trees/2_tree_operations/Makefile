ROOT=../../..
include $(ROOT)/src/Makefile.include

FLAGS += -D_STLA_DEBUG_MEMORY_=NULL
DATA_STRUCTURE=binary_search_tree

all: tree_operations

tree_operations: tree_operations.c data_structure.h $(DATA_STRUCTURE).c $(HEADER_FILES) $(OBJECTS)
	gcc -g $(FLAGS) $(OBJECTS) tree_operations.c $(DATA_STRUCTURE).c -o tree_operations -DDATA_STRUCTURE=\"$(DATA_STRUCTURE)\"

clean:
	rm -rf tree_operations *~ *.dSYM
