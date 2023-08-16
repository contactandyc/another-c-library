lib_LIBRARIES = libac-core.a libac-json.a libac-io.a libac-connect.a libac-utils.a

include ../common.am

# libac-core
libac_core_a_SOURCES = ac-core/ac_allocator.c\
    ac-core/ac_buffer.c\
    ac-core/ac_conv.c\
    ac-core/ac_map.c\
    ac-core/ac_pool.c\
    ac-core/ac_timer.c

core_headers = ../include/ac_allocator.h\
    ../include/ac_buffer.h\
    ../include/ac_common.h\
    ../include/ac_conv.h\
    ../include/ac_map.h\
    ../include/ac_pool.h\
    ../include/ac_search.h\
    ../include/ac_sort.h\
    ../include/ac_timer.h

coredir = $(includedir)/ac-core
core_HEADERS = ../include/ac-core/ac_buffer.h\
    ../include/ac-core/ac_map.h\
    ../include/ac-core/ac_pool.h\
    ../include/ac-core/ac_search.h\
    ../include/ac-core/ac_sort.h

mapdir = $(includedir)/ac-core/ac_map
map_HEADERS = ../include/ac-core/ac_map/ac_map_arg_m.h\
    ../include/ac-core/ac_map/ac_map_compare_arg_m.h\
    ../include/ac-core/ac_map/ac_map_compare_m.h\
    ../include/ac-core/ac_map/ac_map_m.h\
    ../include/ac-core/ac_map/ac_map2_arg_m.h\
    ../include/ac-core/ac_map/ac_map2_compare_arg_m.h\
    ../include/ac-core/ac_map/ac_map2_compare_m.h\
    ../include/ac-core/ac_map/ac_map2_m.h

searchdir = $(includedir)/ac-core/ac_search
search_HEADERS = ../include/ac-core/ac_search/ac_search_arg_m.h\
    ../include/ac-core/ac_search/ac_search_compare_arg_m.h\
    ../include/ac-core/ac_search/ac_search_compare_m.h\
    ../include/ac-core/ac_search/ac_search_m.h

sortdir = $(includedir)/ac-core/ac_sort
sort_HEADERS = ../include/ac-core/ac_sort/ac_sort_arg_m.h\
    ../include/ac-core/ac_sort/ac_sort_aux.h\
    ../include/ac-core/ac_sort/ac_sort_compare_arg_m.h\
    ../include/ac-core/ac_sort/ac_sort_compare_m.h\
    ../include/ac-core/ac_sort/ac_sort_m.h



# libac-json
libac_json_a_SOURCES = ac-json/ac_json.c
# libac_json_a_LIBADD = libac-core.a

json_headers = ../include/ac_json.h

jsondir = $(includedir)/ac-json
json_HEADERS = ../include/ac-json/ac_json.h

# libac-io
libac_io_a_SOURCES = ac-io/ac_in.c\
    ac-io/ac_in_base.c\
    ac-io/ac_io.c\
    ac-io/ac_out.c\
    ac-io/ac_lz4.c\
    ac-io/lz4/lz4.c\
    ac-io/lz4/lz4hc.c\
    ac-io/lz4/xxhash.c\
    ac-io/ac_md5.c\
    ac-io/md5/md5.c

# libac_io_a_LIBADD = libac-core.a libac-json.a

io_headers = ../include/ac_in.h\
    ../include/ac_in_base.h\
    ../include/ac_io.h\
    ../include/ac_out.h\
    ../include/ac_lz4.h\
    ../include/ac_md5.h

iodir = $(includedir)/ac-io
io_HEADERS = ../include/ac-io/ac_in.h\
    ../include/ac-io/ac_in_base.h\
    ../include/ac-io/ac_out.h



# libac-connect

libac_connect_a_SOURCES = ac-connect/ac_cgi.c\
    ac-connect/ac_client.c\
    ac-connect/ac_http_parser.c\
    ac-connect/ac_serve.c\
    ac-connect/llhttp/llhttp.c
# libac_connect_a_LIBADD = libac-core.a libac-json.a libac-io.a

connect_headers = ../include/ac_cgi.h\
    ../include/ac_client.h\
    ../include/ac_http_parser.h\
    ../include/ac_serve.h

connectdir = $(includedir)/ac-connect
connect_HEADERS = ../include/ac-connect/llhttp.h

# libac-utils

libac_utils_a_SOURCES = ac-utils/ac_file_sync.c
# libac_utils_a_LIBADD = libac-core.a libac-json.a libac-io.a

utils_headers = ../include/ac_file_sync.h

# headers for top level directory

include_HEADERS = ${core_headers} ${io_headers} ${json_headers} ${utils_headers} ${connect_headers}
