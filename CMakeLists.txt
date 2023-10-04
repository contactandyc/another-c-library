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

include_directories("/usr/local/include")

# include(CheckIncludeFile)
# check_include_file("the-macro-library/macro_sort.h" HAVE_MACRO_SORT)

find_path(MACRO_SORT_HEADER NAMES "macro_sort.h" PATHS "/usr/local/include/the-macro-library")
if(NOT MACRO_SORT_HEADER)
    message(FATAL_ERROR "The Macro Library found at https://github.com/contactandyc/the-macro-library is not installed")
endif()

add_subdirectory(src)
add_subdirectory(examples)

# Install README, AUTHORS, etc.
install(FILES README.md AUTHORS NEWS.md CHANGELOG.md LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})

# Install header files
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/another-c-library DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
