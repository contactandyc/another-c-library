ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -D_AC_DEBUG_MEMORY_=NULL -lz
PROGRAMS=demo1 scheduler user_ratings_to_binary get_correlations

all: $(PROGRAMS) examples

$(PROGRAMS): %: %.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) $< -o $@

examples:
	./demo1 demo1.c
	./user_ratings_to_binary ~/Downloads/netflix/download/training_set/mv 17770 entries

clean:
	rm -rf *~ *.dSYM demo1
