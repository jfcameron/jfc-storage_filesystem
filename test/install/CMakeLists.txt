# © Joseph Cameron - MIT License

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(jfc-storage_filesystem_consumer LANGUAGES CXX)

find_package(jfc-storage_filesystem 1 REQUIRED)

add_executable(consumer main.cpp)

target_link_libraries(consumer PRIVATE jfc::storage_filesystem)

if (TARGET jfc::storage_user_directories)
    target_link_libraries(consumer PRIVATE jfc::storage_user_directories)
endif()

set_target_properties(consumer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<0:>")
