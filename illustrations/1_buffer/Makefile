all: hello_buffer examples

hello_buffer: hello_buffer.c buffer.c buffer.h
	gcc hello_buffer.c buffer.c -o hello_buffer

examples:
	./hello_buffer

clean:
	rm -f hello_buffer *~
