cmake_minimum_required(VERSION 3.13)

set(OPENCM3_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libopencm3)

if (${BOARD} STREQUAL "bluepill")
	set(BSP f1)
	set(CPU_TYPE stm32f103x8)
elseif(${BOARD} STREQUAL "stm32h743")
	set(BSP h7)
	set(CPU_TYPE ${BOARD})
else()
	message(FATAL_ERROR "Unknown board: ${BOARD}")
endif()
############################## libopencm3 ##############################
include(cmake/libopencm3.cmake)
############################## FreeRTOS ##############################
aux_source_directory(freertos/src/src FR_SRCS)
add_library(freertos OBJECT
	${FR_SRCS}
	freertos/port/${BOARD}/port.c
	freertos/port/MemMang/heap_4.c
	freertos/port/printf-stdarg.c
)
target_include_directories(freertos PUBLIC
	freertos/src/include
	freertos/port/${BOARD}
)

add_library(cio OBJECT
	common/io.c
)

target_link_libraries(cio PRIVATE opencm3)

add_executable(main) 
target_link_libraries(main PUBLIC opencm3 freertos cio)

target_link_libraries(main PUBLIC optimized c)
target_link_libraries(main PUBLIC optimized gcc)
target_link_libraries(main PUBLIC optimized nosys)

get_property(LDSCRIPT_PATH TARGET opencm3 PROPERTY LINK_PATH)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}  -T ${CMAKE_CURRENT_BINARY_DIR}/${BOARD}.ld -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/main.map")

add_custom_command(TARGET main POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -Obinary ${CMAKE_CURRENT_BINARY_DIR}/main.elf ${CMAKE_CURRENT_BINARY_DIR}/main.bin
	COMMENT "Creating binary output main.bin"
)

add_custom_command(TARGET main POST_BUILD
	COMMAND ${CMAKE_OBJDUMP} -SD ${CMAKE_CURRENT_BINARY_DIR}/main.elf > ${CMAKE_CURRENT_BINARY_DIR}/main.lst
	COMMENT "Creating dump output main.lst"
)

add_custom_target(flash-dap
	VERBATIM COMMAND openocd
	-f interface/cmsis-dap.cfg
	-f target/stm32f1x.cfg
	-c "program ${CMAKE_CURRENT_BINARY_DIR}/main.elf verify reset exit"
	DEPENDS main
)

add_custom_target(flash-stlink
	VERBATIM COMMAND openocd
	-f interface/stlink-v2.cfg
	-f target/stm32f1x.cfg
	-c "program ${CMAKE_CURRENT_BINARY_DIR}/main.elf verify reset exit"
	DEPENDS main
)

