ROOT=..
include $(ROOT)/src/Makefile.include

FLAGS += -D_AC_DEBUG_MEMORY_=NULL -lpthread -lm -lz

word_demo: word_demo.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) word_demo.c -o word_demo

clean:
	rm -rf *~ *.dSYM word_demo
