## cpp analyzer code
Simple c++ code parser based on clang

### Prerequisites
Install clang-dev version >=4.0
-[clang+llvm](http://releases.llvm.org/)
-[flatbuffers](https://github.com/google/flatbuffers)

### Compilation
To compile just run
```bash
git clone --recursive --shallow-submodule https://github.com/fnjeneza/code-scanner.git
mkdir build && cd build
cmake -G <Generator> .. -DLLVM_PREFIX=path/to/llvm/dir>
```
