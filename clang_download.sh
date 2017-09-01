#!/bin/bash

echo "llvm installation"

set -e

VERSION=4.0.1
LLVM=llvm-$VERSION
CLANG=cfe-$VERSION
LLVM_ARCHIVE=$LLVM.src.tar.xz
CLANG_ARCHIVE=$CLANG.src.tar.xz
NB_CPU=`cat /proc/cpuinfo | grep processor | wc -l`
CURRENT=$PWD

cd /tmp
wget -c http://releases.llvm.org/$VERSION/$LLVM_ARCHIVE
if [ -d $LLVM.src ]; then
    rm -rf $LLVM.src
fi
tar xvf $LLVM_ARCHIVE

wget -c http://releases.llvm.org/$VERSION/$CLANG_ARCHIVE
if  [-d $CLANG.src ]; then
    rm -rf $CLANG.src
fi
tar xvf $CLANG_ARCHIVE
mv $CLANG.src $LLVM.src/tools/clang

mkdir $CURRENT/clang-build && cd $CURRENT/clang-build

cmake -G Ninja \
	-DCMAKE_BUILD_TYPE=Release \
    -DLLVM_INCLUDE_TESTS=OFF -DLLVM_BUILD_TOOLS=OFF \
    -DCLANG_INCLUDE_DOCS=OFF -DCLANG_INCLUDE_TESTS=OFF -DCLANG_BUILD_TOOLS=OFF \
    -DCLANG_ENABLE_STATIC_ANALYZER=OFF -DCLANG_ENABLE_ARCMT=OFF \
	/tmp/$LLVM.src/

ninja libclang
