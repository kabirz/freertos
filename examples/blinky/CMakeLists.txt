cmake_minimum_required(VERSION 3.13)
project(blinky)

add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/main)

target_sources(main PRIVATE main.c)


