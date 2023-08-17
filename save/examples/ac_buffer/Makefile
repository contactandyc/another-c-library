ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -D_AC_DEBUG_MEMORY_=NULL -lz
PROGRAMS = ac_buffer_init ac_buffer_pool_init ac_buffer_clear ac_buffer_destroy
PROGRAMS += ac_buffer_data ac_buffer_length
PROGRAMS += ac_buffer_resize ac_buffer_shrink_by
PROGRAMS += ac_buffer_alloc ac_buffer_append_ualloc
PROGRAMS += ac_buffer_append ac_buffer_appendc ac_buffer_appendf
PROGRAMS += ac_buffer_appendvf ac_buffer_appendn ac_buffer_appends
PROGRAMS += ac_buffer_set ac_buffer_setc ac_buffer_setf ac_buffer_setvf
PROGRAMS += ac_buffer_setn ac_buffer_sets

all: $(PROGRAMS) examples gitignore

$(PROGRAMS): %: %.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) $< -o $@

examples: $(PROGRAMS)
	@echo Running the examples
	@echo
	./ac_buffer_init
	@echo
	./ac_buffer_pool_init
	@echo
	./ac_buffer_clear
	@echo
	./ac_buffer_data
	@echo
	./ac_buffer_length
	@echo
	./ac_buffer_destroy
	@echo
	./ac_buffer_append
	@echo
	./ac_buffer_appendc
	@echo
	./ac_buffer_appendf
	@echo
	./ac_buffer_appendvf
	@echo
	./ac_buffer_appendn
	@echo
	./ac_buffer_appends
	@echo
	./ac_buffer_set
	@echo
	./ac_buffer_setc
	@echo
	./ac_buffer_setf
	@echo
	./ac_buffer_setvf
	@echo
	./ac_buffer_setn
	@echo
	./ac_buffer_sets
	@echo
	./ac_buffer_resize
	@echo
	./ac_buffer_shrink_by
	@echo
	./ac_buffer_alloc
	@echo
	./ac_buffer_append_ualloc

clean:
	rm -rf *~ *.dSYM $(PROGRAMS)

gitignore:
	@echo "*~" > .gitignore
	@echo "*.dSYM" >> .gitignore
	@for prog in $(PROGRAMS) ; do \
		echo $$prog >> .gitignore; \
	done
