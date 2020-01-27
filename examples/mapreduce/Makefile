ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -g -D_AC_DEBUG_MEMORY_=NULL -lz
PROGRAMS = list_files dump_files_1 dump_files lowercase_and_tokenize sort_tokens sort_tokens_and_display

all: $(PROGRAMS) examples gitignore

$(PROGRAMS): %: %.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) $< -o $@

examples: $(PROGRAMS)
	@echo Running the examples
	@echo
	./list_files . c
	./dump_files sample txt,lz4,gz
	./lowercase_and_tokenize . txt
	./sort_tokens ../.. c,h,md
	./sort_tokens_and_display ../.. c,h,md > sorted_tokens.txt
	head sorted_tokens.txt

clean:
	rm -rf *~ *.dSYM $(PROGRAMS)

gitignore:
	@echo "*~" > .gitignore
	@echo "*.dSYM" >> .gitignore
	@for prog in $(PROGRAMS) ; do \
		echo $$prog >> .gitignore; \
	done
