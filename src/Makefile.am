AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -I$(top_srcdir)/src @AM_CFLAGS@

lib_LIBRARIES = libac.a

libac_a_SOURCES =	ac_allocator.c\
			ac_async_buffer.c\
			ac_buffer.c\
			ac_cgi.c\
			ac_conv.c\
			llhttp/llhttp.c\
			ac_json.c\
			ac_map.c\
			ac_object_pipe.c\
			ac_pool.c\
			ac_threaded_pipe.c\
			ac_timer.c

libac_adir=$(includedir)
libac_a_HEADERS = ac_allocator.h\
			ac_async_buffer.h\
			ac_buffer.h\
			ac_cgi.h\
			ac_common.h\
			ac_conv.h\
			ac_json.h\
			ac_map.h\
			ac_object_pipe.h\
			ac_pool.h\
			ac_search.h\
			ac_sort.h\
			ac_threaded_pipe.h\
			ac_timer.h\
			llhttp/llhttp.h\
			impl/ac_buffer.h\
			impl/ac_json.h\
			impl/ac_map.h\
			impl/ac_pool.h\
			impl/ac_search.h\
			impl/ac_sort.h\
