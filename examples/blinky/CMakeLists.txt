cmake_minimum_required(VERSION 3.13)
project(blinky)

add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/_build_)

add_executable(blinky main.c)
target_link_libraries(blinky PUBLIC opencm3)
 
get_property(LDSCRIPT_PATH TARGET opencm3 PROPERTY LINK_PATH)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}  -T ${CMAKE_CURRENT_SOURCE_DIR}/link.ld -L ${LDSCRIPT_PATH}")
