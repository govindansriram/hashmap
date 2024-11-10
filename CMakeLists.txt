# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.22)

# Set the project name and version
project(hashmap VERSION 0.0.0 DESCRIPTION "hashmap data structure implementation" LANGUAGES C)

# Specify the C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Define the source directory
set(SRC_DIR src)

# Set the output directory for the shared library
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# List of all sources for the shared library
set(SOURCES
        src/unity/unity.c
        src/hash_algorithm/hash_algorithms.c
        src/map/map.c
        include/hash.h)

# SHARED is preferred over static for python integrations
add_library(hashmap SHARED ${SOURCES})

# Include directories for public headers
target_include_directories(hashmap PUBLIC include)

# Use the unique directories for target include directories
target_include_directories(hashmap PRIVATE
        src/hash_algorithm
)

# Define install rules
install(TARGETS hashmap
        DESTINATION lib
)

install(DIRECTORY include/
        DESTINATION include
)

include_directories(src/unity)

enable_testing()
add_executable(test_hash_algo tests/src/hash_algorithms/test_hash_algorithms.c)
target_link_libraries(test_hash_algo PRIVATE hashmap)

add_executable(test_map tests/src/map/test_map.c)
target_link_libraries(test_map PRIVATE hashmap)
