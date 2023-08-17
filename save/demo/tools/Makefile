ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -O3 -D_AC_DEBUG_MEMORY_=NULL -lz
PROGRAMS=side_by_side

all: $(PROGRAMS)

$(PROGRAMS): %: %.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) $< -o $@

clean:
	rm -rf *~ *.dSYM $(PROGRAMS)
