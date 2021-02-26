if [ ! -d out ]; then
	cmake -S . -B out -DCMAKE_TOOLCHAIN_FILE=../../cmake/toolchain_arm_gcc.cmake -DBOARD=bluepill -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -GNinja -DOPENCM3_USE_INTERNAL=OFF
fi
cmake --build out
