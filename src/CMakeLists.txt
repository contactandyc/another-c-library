find_package(ZLIB REQUIRED)
find_package(libuv QUIET)

set(libac_core_a_SOURCES
    ac-core/ac_allocator.c
    ac-core/ac_buffer.c
    ac-core/ac_conv.c
    ac-core/ac_pool.c
    ac-core/ac_timer.c
)
add_library(ac-core STATIC ${libac_core_a_SOURCES})
target_link_libraries(ac-core PRIVATE ZLIB::ZLIB)

set(libac_json_a_SOURCES ac-json/ac_json.c)
add_library(ac-json STATIC ${libac_json_a_SOURCES})

set(libac_io_a_SOURCES
    ac-io/ac_in.c
    ac-io/ac_in_base.c
    ac-io/ac_io.c
    ac-io/ac_out.c
    ac-io/ac_lz4.c
    ac-io/lz4/lz4.c
    ac-io/lz4/lz4hc.c
    ac-io/lz4/xxhash.c
    ac-io/ac_md5.c
    ac-io/md5/md5.c
)
add_library(ac-io STATIC ${libac_io_a_SOURCES})
target_link_libraries(ac-io PRIVATE ZLIB::ZLIB)

find_library(LIBUV_LIBRARY NAMES uv)

if(LIBUV_LIBRARY)
    set(libac_connect_a_SOURCES
        ac-connect/ac_cgi.c
        ac-connect/ac_client.c
        ac-connect/ac_http_parser.c
        ac-connect/ac_serve.c
        ac-connect/llhttp/llhttp.c
    )
    add_library(ac-connect STATIC ${libac_connect_a_SOURCES})
    target_link_libraries(ac-connect PRIVATE ZLIB::ZLIB ${LIBUV_LIBRARY})
else()
    set(libac_connect_a_SOURCES
        ac-connect/ac_cgi.c
    )
    add_library(ac-connect STATIC ${libac_connect_a_SOURCES})
    target_link_libraries(ac-connect PRIVATE ZLIB::ZLIB)
endif()

set(libac_utils_a_SOURCES ac-utils/ac_file_sync.c)
add_library(ac-utils STATIC ${libac_utils_a_SOURCES})
target_link_libraries(ac-utils PRIVATE ZLIB::ZLIB)
