cmake_minimum_required(VERSION 3.13)

project(opencm3)

execute_process(
		COMMAND ${Python3_EXECUTABLE} scripts/genlink.py ld/devices.data ${CPU_TYPE} DEFS
		WORKING_DIRECTORY                ${OPENCM3_PATH}
		OUTPUT_VARIABLE                  BOARD_DEF
)

add_custom_target(gen_ldscript
    WORKING_DIRECTORY ${OPENCM3_PATH}
    VERBATIM COMMAND eval "${CMAKE_C_COMPILER} -E ${BOARD_DEF} -P -E ld/linker.ld.S -o ${CMAKE_CURRENT_BINARY_DIR}/${BOARD}.ld"
)

if (OPENCM3_USE_INTERNAL)
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
add_dependencies(opencm3 libopencm3 gen_ldscript)
set_target_properties(opencm3 PROPERTIES IMPORTED_LOCATION ${OPENCM3_PATH}/lib/libopencm3_stm32${BSP}.a)
set_target_properties(opencm3 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${OPENCM3_PATH}/include)
set_target_properties(opencm3 PROPERTIES INTERFACE_COMPILE_OPTIONS ${BOARD_DEF})

else()
add_library(opencm3 OBJECT
    ${OPENCM3_PATH}/lib/stm32/can.c
    ${OPENCM3_PATH}/lib/stm32/common/adc_common_v1.c
    ${OPENCM3_PATH}/lib/stm32/common/dac_common_all.c
    ${OPENCM3_PATH}/lib/stm32/common/crc_common_all.c
    ${OPENCM3_PATH}/lib/stm32/common/desig_common_v1.c
    ${OPENCM3_PATH}/lib/stm32/common/dac_common_v1.c
    ${OPENCM3_PATH}/lib/stm32/common/dma_common_l1f013.c
    ${OPENCM3_PATH}/lib/stm32/common/desig_common_all.c
    ${OPENCM3_PATH}/lib/stm32/f1/flash.c
    ${OPENCM3_PATH}/lib/stm32/common/exti_common_all.c
    ${OPENCM3_PATH}/lib/stm32/common/flash_common_all.c
    ${OPENCM3_PATH}/lib/stm32/f1/adc.c
    ${OPENCM3_PATH}/lib/stm32/common/flash_common_f.c
    ${OPENCM3_PATH}/lib/stm32/f1/gpio.c
    ${OPENCM3_PATH}/lib/stm32/common/flash_common_f01.c
    ${OPENCM3_PATH}/lib/stm32/common/gpio_common_all.c
    ${OPENCM3_PATH}/lib/stm32/common/i2c_common_v1.c
    ${OPENCM3_PATH}/lib/stm32/common/iwdg_common_all.c
    ${OPENCM3_PATH}/lib/stm32/common/pwr_common_v1.c
    ${OPENCM3_PATH}/lib/stm32/f1/rcc.c
    ${OPENCM3_PATH}/lib/stm32/common/rcc_common_all.c
    ${OPENCM3_PATH}/lib/stm32/f1/rtc.c
    ${OPENCM3_PATH}/lib/stm32/common/spi_common_all.c
    ${OPENCM3_PATH}/lib/stm32/common/spi_common_v1.c
    ${OPENCM3_PATH}/lib/stm32/f1/timer.c
    ${OPENCM3_PATH}/lib/stm32/common/usart_common_all.c
    ${OPENCM3_PATH}/lib/stm32/common/timer_common_all.c
    ${OPENCM3_PATH}/lib/stm32/common/usart_common_f124.c
    ${OPENCM3_PATH}/lib/ethernet/mac_stm32fxx7.c
    ${OPENCM3_PATH}/lib/ethernet/mac.c
    ${OPENCM3_PATH}/lib/ethernet/phy.c
    ${OPENCM3_PATH}/lib/ethernet/phy_ksz80x1.c
    ${OPENCM3_PATH}/lib/usb/usb.c
    ${OPENCM3_PATH}/lib/usb/usb_control.c
    ${OPENCM3_PATH}/lib/usb/usb_standard.c
    ${OPENCM3_PATH}/lib/stm32/common/st_usbfs_core.c
    ${OPENCM3_PATH}/lib/stm32/st_usbfs_v1.c
    ${OPENCM3_PATH}/lib/cm3/vector.c
    ${OPENCM3_PATH}/lib/cm3/scb.c
    ${OPENCM3_PATH}/lib/cm3/systick.c
    ${OPENCM3_PATH}/lib/cm3/nvic.c
    ${OPENCM3_PATH}/lib/cm3/assert.c
    ${OPENCM3_PATH}/lib/cm3/sync.c
    ${OPENCM3_PATH}/lib/cm3/dwt.c
)

target_compile_options(opencm3 PUBLIC ${BOARD_DEF})
target_include_directories(opencm3 PUBLIC ${OPENCM3_PATH}/include)

add_custom_target(irq2nvic
    WORKING_DIRECTORY ${OPENCM3_PATH}
    COMMAND scripts/irq2nvic_h ./include/libopencm3/stm32/f1/irq.json
)


add_dependencies(opencm3 irq2nvic gen_ldscript)
endif()

