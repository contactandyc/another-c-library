ROOT=../../..
include $(ROOT)/src/Makefile.include

FLAGS += -D_AC_DEBUG_MEMORY_=NULL

all: test_data_structure examples

test_data_structure: test_data_structure.c $(HEADER_FILES) $(OBJECTS)
	gcc $(FLAGS) $(OBJECTS) test_data_structure.c -o test_data_structure

examples:
	./test_data_structure ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 10000

clean:
	rm -rf test_data_structure test_data_structure.dSYM *~
