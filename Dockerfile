FROM gcc:8.2
RUN apt update && \
    apt install -y \
    --no-install-recommends \
    ninja-build \
    cmake \
    cmake-curses-gui

ARG REMOTE_ARCHIVE=clang+llvm-7.0.1-x86_64-linux-gnu-ubuntu-18.04.tar.xz
ARG ARCHIVE=/tmp/clang_llvm.tar.xz
RUN curl -fSL -o $ARCHIVE http://releases.llvm.org/7.0.1/$REMOTE_ARCHIVE
WORKDIR /opt/clang_llvm
RUN tar xvf $ARCHIVE --strip-components=1
RUN rm $ARCHIVE

ARG VERSION=1.10.0
ARG FLATC_REMOTE=https://github.com/google/flatbuffers/archive/v${VERSION}.zip
ARG FLATC_ARCHIVE=flatbuffers.zip
WORKDIR /tmp/workspace
RUN curl -k -fSL -o $FLATC_ARCHIVE $FLATC_REMOTE
RUN unzip $FLATC_ARCHIVE && \
    mkdir build && cd build && \
    cmake -G Ninja ../flatbuffers-${VERSION} -DCMAKE_BUILD_TYPE=Release \
    && ninja \
    && ninja install
RUN rm -r /tmp/workspace

WORKDIR /app
