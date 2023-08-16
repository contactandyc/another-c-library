cmake_minimum_required(VERSION 3.10)
project(anotherclibrary VERSION 1.0)

option(DEBUG "Enable debugging" OFF)
option(ADDRESS_SANITIZER "Enable Address Sanitizer" OFF)
option(MEMORY_CHECK "Enable Memory Check with given value" OFF)
option(MEMORY_CHECK_FILE "Enable Memory Check with given file" NO)

set(CMAKE_INSTALL_INCLUDEDIR include)
set(CMAKE_INSTALL_DOCDIR share/doc/anotherclibrary)
set(CMAKE_C_STANDARD 99)

if(DEBUG)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I${CMAKE_SOURCE_DIR}/include -g -O0")
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I${CMAKE_SOURCE_DIR}/include -O3")
endif()

if(ADDRESS_SANITIZER)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
endif()

if(MEMORY_CHECK)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_AC_MEMORY_CHECK_=NULL")
endif()

if(MEMORY_CHECK_FILE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_AC_MEMORY_CHECK_='\"${MEMORY_CHECK_FILE}\"'")
endif()

add_subdirectory(lib)
add_subdirectory(examples)

# Install README, AUTHORS, etc.
install(FILES README.md AUTHORS NEWS.md CHANGELOG.md LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})
