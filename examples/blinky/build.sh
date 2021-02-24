if [ ! -d out ]; then
	cmake -S . -B out -DCMAKE_TOOLCHAIN_FILE=../../cmake/toolchain_arm_gcc.cmake -DBOARD=stm32f103 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
fi
cmake --build out
