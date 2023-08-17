find_package(ZLIB REQUIRED)
find_package(libuv QUIET)

set(libac_core_a_SOURCES
    ac-core/ac_allocator.c
    ac-core/ac_buffer.c
    ac-core/ac_conv.c
    ac-core/ac_map.c
    ac-core/ac_pool.c
    ac-core/ac_timer.c
)
add_library(ac-core STATIC ${libac_core_a_SOURCES})
target_link_libraries(ac-core PRIVATE ZLIB::ZLIB)

set(core_headers
    ${CMAKE_SOURCE_DIR}/include/ac_allocator.h
    ${CMAKE_SOURCE_DIR}/include/ac_buffer.h
    ${CMAKE_SOURCE_DIR}/include/ac_common.h
    ${CMAKE_SOURCE_DIR}/include/ac_conv.h
    ${CMAKE_SOURCE_DIR}/include/ac_map.h
    ${CMAKE_SOURCE_DIR}/include/ac_pool.h
    ${CMAKE_SOURCE_DIR}/include/ac_search.h
    ${CMAKE_SOURCE_DIR}/include/ac_sort.h
    ${CMAKE_SOURCE_DIR}/include/ac_timer.h
)

set(coredir ${CMAKE_INSTALL_INCLUDEDIR}/ac-core)
set(core_HEADERS
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_buffer.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_map.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_pool.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_search.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_sort.h
)
install(FILES ${core_HEADERS} DESTINATION ${coredir})

set(mapdir ${CMAKE_INSTALL_INCLUDEDIR}/ac-core/ac_map)
set(map_HEADERS
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_map/ac_map_arg_m.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_map/ac_map_compare_arg_m.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_map/ac_map_compare_m.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_map/ac_map_m.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_map/ac_map2_arg_m.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_map/ac_map2_compare_arg_m.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_map/ac_map2_compare_m.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_map/ac_map2_m.h
)
install(FILES ${map_HEADERS} DESTINATION ${mapdir})

set(searchdir ${CMAKE_INSTALL_INCLUDEDIR}/ac-core/ac_search)
set(search_HEADERS
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_search/ac_search_arg_m.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_search/ac_search_compare_arg_m.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_search/ac_search_compare_m.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_search/ac_search_m.h
)
install(FILES ${search_HEADERS} DESTINATION ${searchdir})

set(sortdir ${CMAKE_INSTALL_INCLUDEDIR}/ac-core/ac_sort)
set(sort_HEADERS
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_sort/ac_sort_arg_m.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_sort/ac_sort_aux.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_sort/ac_sort_compare_arg_m.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_sort/ac_sort_compare_m.h
    ${CMAKE_SOURCE_DIR}/include/ac-core/ac_sort/ac_sort_m.h
)
install(FILES ${sort_HEADERS} DESTINATION ${sortdir})


set(libac_json_a_SOURCES ac-json/ac_json.c)
add_library(ac-json STATIC ${libac_json_a_SOURCES})

set(json_headers ${CMAKE_SOURCE_DIR}/include/ac_json.h)

set(jsondir ${CMAKE_INSTALL_INCLUDEDIR}/ac-json)
set(json_HEADERS ${CMAKE_SOURCE_DIR}/include/ac-json/ac_json.h)
install(FILES ${json_HEADERS} DESTINATION ${jsondir})

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

set(io_headers
    ${CMAKE_SOURCE_DIR}/include/ac_in.h
    ${CMAKE_SOURCE_DIR}/include/ac_in_base.h
    ${CMAKE_SOURCE_DIR}/include/ac_io.h
    ${CMAKE_SOURCE_DIR}/include/ac_out.h
    ${CMAKE_SOURCE_DIR}/include/ac_lz4.h
    ${CMAKE_SOURCE_DIR}/include/ac_md5.h
)

set(iodir ${CMAKE_INSTALL_INCLUDEDIR}/ac-io)
set(io_HEADERS
    ${CMAKE_SOURCE_DIR}/include/ac-io/ac_in.h
    ${CMAKE_SOURCE_DIR}/include/ac-io/ac_in_base.h
    ${CMAKE_SOURCE_DIR}/include/ac-io/ac_out.h
)
install(FILES ${io_HEADERS} DESTINATION ${iodir})

if(libuv_FOUND)
    set(libac_connect_a_SOURCES
        ac-connect/ac_cgi.c
        ac-connect/ac_client.c
        ac-connect/ac_http_parser.c
        ac-connect/ac_serve.c
        ac-connect/llhttp/llhttp.c
    )
    add_library(ac-connect STATIC ${libac_connect_a_SOURCES})
    target_link_libraries(ac-connect PRIVATE ZLIB::ZLIB libuv::libuv)

    set(connect_headers
        ${CMAKE_SOURCE_DIR}/include/ac_cgi.h
        ${CMAKE_SOURCE_DIR}/include/ac_client.h
        ${CMAKE_SOURCE_DIR}/include/ac_http_parser.h
        ${CMAKE_SOURCE_DIR}/include/ac_serve.h
    )

    set(connectdir ${CMAKE_INSTALL_INCLUDEDIR}/ac-connect)
    set(connect_HEADERS ${CMAKE_SOURCE_DIR}/include/ac-connect/llhttp.h)
    install(FILES ${connect_HEADERS} DESTINATION ${connectdir})
else()
    set(libac_connect_a_SOURCES
        ac-connect/ac_cgi.c
    )
    add_library(ac-connect STATIC ${libac_connect_a_SOURCES})
    target_link_libraries(ac-connect PRIVATE ZLIB::ZLIB)

    set(connect_headers
        ${CMAKE_SOURCE_DIR}/include/ac_cgi.h
    )
endif()


set(libac_utils_a_SOURCES ac-utils/ac_file_sync.c)
add_library(ac-utils STATIC ${libac_utils_a_SOURCES})
target_link_libraries(ac-utils PRIVATE ZLIB::ZLIB)

set(utils_headers ${CMAKE_SOURCE_DIR}/include/ac_file_sync.h)


# headers for top level directory

set(include_HEADERS
    ${core_headers}
    ${json_headers}
    ${io_headers}
    ${connect_headers}
    ${utils_headers}
)

install(FILES ${include_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
