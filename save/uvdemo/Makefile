UV=1
ROOT=..
include $(ROOT)/src/Makefile.include

FLAGS += -g -D_AC_DEBUG_MEMORY_=NULL
PROGRAMS=json_service uv_client uvdemo1

all: $(PROGRAMS) examples

$(PROGRAMS): %: %.c
	gcc $(FLAGS) $(OBJECTS) $(UV_OBJECTS) $< -o $@ -luv

examples:
	time ./uvdemo1 ../demo/names.txt

web: clean-webserver webserver

clean-webserver:
	rm -f webserver

clean:
	rm -rf *~ *.dSYM uvdemo1 tcpdemo webserver
