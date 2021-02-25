cmake_minimum_required(VERSION 3.13)

set(OPENCM3_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libopencm3)

if (${BOARD} STREQUAL "stm32f103")
	set(BSP f1)
	set(BOARD_DEF STM32F1)
elseif(${BOARD} STREQUAL "stm32h743")
	set(BSP h7)
	set(BOARD_DEF STM32H7)
else()
	message(FATAL_ERROR "Unknown board: ${BOARD}")
endif()
############################## libopencm3 ##############################
cmake_host_system_information(RESULT NCPU QUERY NUMBER_OF_LOGICAL_CORES)
include(ExternalProject)
ExternalProject_Add(
		libopencm3
		PREFIX ${OPENCM3_PATH}
		SOURCE_DIR ${OPENCM3_PATH}
		BINARY_DIR ${OPENCM3_PATH}
		CONFIGURE_COMMAND ""
		BUILD_COMMAND
		make TARGETS=stm32/${BSP} -j ${NCPU}
		INSTALL_COMMAND ""
		BUILD_BYPRODUCTS ${OPENCM3_PATH}/lib/libopencm3_stm32${BSP}.a
)

add_library(opencm3 STATIC IMPORTED GLOBAL)
add_dependencies(
	opencm3
	libopencm3
)
set_target_properties(opencm3 PROPERTIES IMPORTED_LOCATION ${OPENCM3_PATH}/lib/libopencm3_stm32${BSP}.a)
set_target_properties(opencm3 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${OPENCM3_PATH}/include)
set_target_properties(opencm3 PROPERTIES INTERFACE_COMPILE_DEFINITIONS ${BOARD_DEF})
set_target_properties(opencm3 PROPERTIES LINK_PATH ${OPENCM3_PATH}/lib)

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
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}  -T ${CMAKE_CURRENT_SOURCE_DIR}/ld/${BOARD}.ld -L ${LDSCRIPT_PATH} -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/main.map")

add_custom_command(TARGET main POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -Obinary ${CMAKE_CURRENT_BINARY_DIR}/main.elf ${CMAKE_CURRENT_BINARY_DIR}/main.bin
	COMMENT "Creating binary output main.bin"
)

add_custom_command(TARGET main POST_BUILD
	COMMAND ${CMAKE_OBJDUMP} -SD ${CMAKE_CURRENT_BINARY_DIR}/main.elf > ${CMAKE_CURRENT_BINARY_DIR}/main.lst
	COMMENT "Creating dump output main.lst"
)

add_custom_target(flash
	COMMAND openocd-zephyr
	-f interface/cmsis-dap.cfg
	-f target/stm32f1x.cfg
	-c init -c halt
	-c "flash write_image erase ${CMAKE_CURRENT_BINARY_DIR}/main.elf"
	-c reset -c shutdown
	DEPENDS main
)

