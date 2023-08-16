# Specify the include directories
include_directories(${LIBUV_INCLUDE_DIRS})

# Define the executable
add_executable(read_file_into_buffer read_file_into_buffer.c)

# Link the required libraries
target_link_libraries(read_file_into_buffer
    ac-core
    ac-json
    ac-io
    z
    ${LIBUV_LIBRARIES}
)

# Copy the sample.txt file to the build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sample.txt ${CMAKE_CURRENT_BINARY_DIR}/sample.txt COPYONLY)
