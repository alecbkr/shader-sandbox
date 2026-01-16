cd ..
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=./clang-toolchain.cmake

cd build
cmake --build .
cd ../.mytools

