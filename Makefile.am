AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4
lib_LTLIBRARIES = libac.la

libac_la_SOURCES =	src/ac_allocator.c\
			src/ac_async_buffer.c\
			src/ac_buffer.c\
			src/ac_cgi.c\
			src/ac_conv.c\
			src/llhttp/llhttp.c\
			src/ac_map.c\
			src/ac_object_pipe.c\
			src/ac_pool.c\
			src/ac_threaded_pipe.c\
			src/ac_timer.c

libac_la_LIBADD = -luv

include_HEADERS = src/ac_allocator.h\
			src/ac_async_buffer.h\
			src/ac_buffer.h\
			src/ac_cgi.h\
			src/ac_common.h\
			src/ac_conv.h\
			src/ac_map.h\
			src/ac_object_pipe.h\
			src/ac_pool.h\
			src/ac_search.h\
			src/ac_sort.h\
			src/ac_sortl.h\
			src/ac_threaded_pipe.h\
			src/ac_timer.h\
			src/llhttp/llhttp.h\
			src/impl/ac_buffer.h\
			src/impl/ac_map.h\
			src/impl/ac_pool.h\
			src/impl/ac_search.h\
			src/impl/ac_sort.h\
			src/impl/ac_sortl.h

include demo/Makefile.am
