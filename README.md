## cpp analyzer code
Simple c++ code parser based on clang

### Prerequisites
Install clang-dev version >=4.0

On ubuntu this can be installed by using this command!:
```bash
apt install libclang-dev

git clone https://github.com/google/flatbuffers.git
cd flatbuffers
mkdir build && cd build
cmake -G Ninja .. [-DCMAKE_INSTALL_PREFIX=/opt/local]
ninja install
```
It is also possible to compile it from source

### Compilation
To compile just run
```bash
mkdir build && cd build
cmake -G <Generator> .. -DLLVM_PREFIX=<path/to/llvm/dir>
```
