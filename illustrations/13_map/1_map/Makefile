ROOT=../../..
include $(ROOT)/src/Makefile.include

FLAGS += -D_STLA_DEBUG_MEMORY_=NULL

all: test_data_structure

test_data_structure: test_data_structure.c $(HEADER_FILES) $(OBJECTS)
	gcc $(FLAGS) $(OBJECTS) test_data_structure.c -o test_data_structure

clean:
	rm -rf test_data_structure test_data_structure.dSYM *~
