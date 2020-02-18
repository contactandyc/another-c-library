ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -O3 -D_AC_DEBUG_MEMORY_=NULL -lz -luv
PROGRAMS = ac_http

OBJECTS += $(UV_OBJECTS)
HEADER_FILES += $(UV_HEADER_FILES)

all: $(PROGRAMS) examples gitignore

$(PROGRAMS): %: %.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) $< -o $@

examples: $(PROGRAMS)
	@echo Running the examples

clean:
	rm -rf *~ *.dSYM $(PROGRAMS)

gitignore:
	@echo "*~" > .gitignore
	@echo "*.dSYM" >> .gitignore
	@for prog in $(PROGRAMS) ; do \
		echo $$prog >> .gitignore; \
	done
