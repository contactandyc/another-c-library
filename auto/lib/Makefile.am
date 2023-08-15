lib_LIBRARIES = libac-core.a libac-io.a libac-json.a libac-utils.a

include ../common.am

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

libac_json_a_SOURCES = ac-json/ac_json.c

json_headers = ../include/ac_json.h

jsondir = $(includedir)/ac-json
json_HEADERS = ../include/ac-json/ac_json.h

libac_utils_a_SOURCES = ac-utils/ac_timer.c

utils_headers = ../include/ac_timer.h

libac_core_a_SOURCES = ac-core/ac_allocator.c\
    ac-core/ac_buffer.c\
    ac-core/ac_conv.c\
    ac-core/ac_map.c\
    ac-core/ac_pool.c

core_headers = ../include/ac_allocator.h\
    ../include/ac_buffer.h\
    ../include/ac_common.h\
    ../include/ac_conv.h\
    ../include/ac_map.h\
    ../include/ac_pool.h\
    ../include/ac_search.h\
    ../include/ac_sort.h

include_HEADERS = ${core_headers} ${io_headers} ${json_headers} ${utils_headers}

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