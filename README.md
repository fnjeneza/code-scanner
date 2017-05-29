Install clang-dev version >=4.0

apt install libclang-dev

To compile just run

mkdir build && cd build
cmake -G Generator .. -DCLANG_PREFIX=<path clang dir>
