ROOT=..
include $(ROOT)/src/Makefile.include

FLAGS += -D_AC_DEBUG_MEMORY_=NULL -lz

scheduler: training_set scheduler.c aux_methods.h aux_methods.c convert_ratings.h convert_ratings.c sort_entries_by_user_and_item.h sort_entries_by_user_and_item.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) scheduler.c aux_methods.c convert_ratings.c sort_entries_by_user_and_item.c -o scheduler -lpthread -lm

training_set: training_set.tgz
	tar xf training_set.tgz

clean:
	rm -rf *~ *.dSYM scheduler
