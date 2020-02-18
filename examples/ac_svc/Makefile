ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -O3 -lz -luv
# FLAGS += -g -D_AC_DEBUG_MEMORY_=NULL -lz -luv 
PROGRAMS = ac_svc

OBJECTS += $(UV_OBJECTS)
HEADER_FILES += $(UV_HEADER_FILES)

all: $(PROGRAMS) examples gitignore

$(PROGRAMS): %: %.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) $< -o $@

examples: $(PROGRAMS)
	@echo Running the examples
	@echo
	./ac_svc

clean:
	rm -rf *~ *.dSYM sort*.txt sorted_tokens $(PROGRAMS)

gitignore:
	@echo "*~" > .gitignore
	@echo "*.dSYM" >> .gitignore
	@for prog in $(PROGRAMS) ; do \
		echo $$prog >> .gitignore; \
	done
