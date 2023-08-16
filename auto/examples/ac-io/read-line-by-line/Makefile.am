include ../../../common.am

noinst_PROGRAMS = read_line_by_line
read_line_by_line_SOURCES = read_line_by_line.c
read_line_by_line_LDADD = ../../../lib/libac-core.a ../../../lib/libac-json.a ../../../lib/libac-io.a -lz $(LIBUV_LIBS)

localdatadir = $(builddir)
localdata_DATA = sample.txt

all-local: $(localdata_DATA)

all-local: $(localdata_DATA)
	for file in $(localdata_DATA); do \
	  $(INSTALL) -m 644 $(srcdir)/$$file $(localdatadir); \
	done

uninstall-local:
	for file in $(localdata_DATA); do \
	  $(RM) $(localdatadir)/$$file; \
	done