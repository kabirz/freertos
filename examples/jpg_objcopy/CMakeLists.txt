cmake_minimum_required(VERSION 3.16)

project(gen_array)

add_executable(gen_array main.c)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pic.o
    COMMAND objcopy -I binary -O elf64-x86-64 -B i386 pic.jpg ${CMAKE_CURRENT_BINARY_DIR}/pic.o
    --redefine-sym _binary_pic_jpg_start=image_start
    --redefine-sym _binary_pic_jpg_end=image_end
    --redefine-sym _binary_pic_jpg_size=image_size
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS pic.jpg
)

add_custom_target(gen_pic DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/pic.o)
target_link_libraries(gen_array ${CMAKE_CURRENT_BINARY_DIR}/pic.o -Wl,-Map=gen_pic.map)

add_dependencies(gen_array gen_pic)

