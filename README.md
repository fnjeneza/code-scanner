## cpp analyzer code
Simple c++ code parser based on clang

### Prerequisites
Install clang-dev version >=4.0

On ubuntu this can be installed by using this command!:
```bash
mkdir build && cd build
../scripts/flatc_install.py
../scripts/ubuntu_install.py
```
It is also possible to compile it from source

### Compilation
To compile just run
```bash
cd build
cmake -G <Generator> .. -DLLVM_PREFIX=<path/to/llvm/dir>
```
