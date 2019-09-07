ROOT=../../..
OBJECTS=$(ROOT)/src/stla_pool.c $(ROOT)/src/stla_buffer.c
FLAGS=-I$(ROOT)/src -O3

PROGRAMS=hello_buffer_sets hello_buffer_set hello_buffer_appends hello_buffer_append hello_buffer_appendc hello_buffer_appendn hello_buffer_setf hello_buffer_appendf hello_buffer_alloc hello_buffer_append_alloc hello_buffer

all: $(PROGRAMS) examples

$(PROGRAMS): %: %.c
	gcc $(FLAGS) $(OBJECTS) $< -o $@

examples:
	@for cmd in $(PROGRAMS); do \
		echo "./$$cmd"; \
		./$$cmd; \
	done

clean:
	for cmd in $(PROGRAMS); do \
		rm -f $$cmd; \
	done
	rm -f *~
