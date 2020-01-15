ROOT=..
include $(ROOT)/src/Makefile.include

FLAGS += -D_AC_DEBUG_MEMORY_=NULL -lpthread -lm -lz

scheduler: training_set scheduler.c aux_methods.h $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) scheduler.c -o scheduler

training_set: training_set.tgz
	tar xf training_set.tgz

clean:
	rm -rf *~ *.dSYM scheduler
