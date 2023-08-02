ROOT=../..
include $(ROOT)/src/Makefile.include

# FLAGS += -O3 -D_AC_DEBUG_MEMORY_=NULL -lz
FLAGS += -g -D_AC_DEBUG_MEMORY_=NULL -lz
OBJECTS += ac_topic.c
HEADER_FILES += ac_topic.h
PROGRAMS=topic

all: $(PROGRAMS)

$(PROGRAMS): %: %.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) $< -o $@

clean:
	rm -rf *~ *.dSYM $(PROGRAMS)