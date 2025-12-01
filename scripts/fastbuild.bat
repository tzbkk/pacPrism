rm -r build

cmake -B build

cmake --build build && ./build/bin/pacprism.exe