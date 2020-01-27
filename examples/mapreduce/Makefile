ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -g -D_AC_DEBUG_MEMORY_=NULL -lz
PROGRAMS = list_files dump_files_1 dump_files lowercase_and_tokenize sort_tokens
PROGRAMS += sort_tokens_and_display sort_tokens_reduce_and_display
PROGRAMS += sort_tokens_reduce_sort_by_freq_and_display
all: $(PROGRAMS) examples gitignore

$(PROGRAMS): %: %.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) $< -o $@

examples: $(PROGRAMS)
	@echo Running the examples
	@echo
	./list_files . c
	./dump_files sample txt,lz4,gz
	./lowercase_and_tokenize sample txt
	./sort_tokens ../.. c,h,md
	rm sorted_tokens
	./sort_tokens_and_display ../.. c,h,md > sorted_tokens.txt
	head sorted_tokens.txt
	./sort_tokens_reduce_and_display ../.. c,h,md > sorted_tokens.txt
	head sorted_tokens.txt
	./sort_tokens_reduce_sort_by_freq_and_display ../.. c,h,md > sorted_tokens.txt
	head sorted_tokens.txt
	rm sorted_tokens.txt
	
clean:
	rm -rf *~ *.dSYM $(PROGRAMS)

gitignore:
	@echo "*~" > .gitignore
	@echo "*.dSYM" >> .gitignore
	@for prog in $(PROGRAMS) ; do \
		echo $$prog >> .gitignore; \
	done
