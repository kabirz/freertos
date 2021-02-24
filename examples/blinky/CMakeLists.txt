cmake_minimum_required(VERSION 3.13)
project(blinky)

add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/_build_)

add_executable(blinky main.c)
target_link_libraries(blinky PUBLIC opencm3 freertos)
 
get_property(LDSCRIPT_PATH TARGET opencm3 PROPERTY LINK_PATH)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}  -T ${CMAKE_CURRENT_SOURCE_DIR}/link.ld -L ${LDSCRIPT_PATH}")

add_custom_command(TARGET blinky POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -Obinary ${CMAKE_BINARY_DIR}/blinky ${CMAKE_BINARY_DIR}/blinky.bin
	COMMENT "Creating binary output blinky.bin"
)

add_custom_command(TARGET blinky POST_BUILD
	COMMAND ${CMAKE_OBJDUMP} -d -S ${CMAKE_BINARY_DIR}/blinky > ${CMAKE_BINARY_DIR}/blinky.lst
	COMMENT "Creating dump output blinky.lst"
)

