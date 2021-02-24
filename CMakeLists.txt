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
aux_source_directory(free_rtos_src/src FR_SRCS)
add_library(freertos OBJECT
	${FR_SRCS}
	free_rtos_port/${BOARD}/port.c
	free_rtos_port/MemMang/heap_3.c
)
target_include_directories(freertos PUBLIC
	free_rtos_src/include
	free_rtos_port/${BOARD}
)
