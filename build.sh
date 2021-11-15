#!/usr/bin/env bash

apt-get update

# install folly dependencies
apt-get install -y \
    wget \
    git \
    g++ \
    cmake \
    libboost-all-dev \
    libevent-dev \
    libdouble-conversion-dev \
    libgoogle-glog-dev \
    libgflags-dev \
    libiberty-dev \
    liblz4-dev \
    liblzma-dev \
    libsnappy-dev \
    make \
    zlib1g-dev \
    binutils-dev \
    libjemalloc-dev \
    libssl-dev \
    pkg-config \
    libunwind-dev \
    libunwind8-dev \
    libelf-dev \
    libdwarf-dev

# install fmt
git clone https://github.com/fmtlib/fmt.git && \
cd fmt && \
mkdir _build && cd _build && \
cmake .. && \
make -j "$(nproc)" && \
make install && \
cd ../..

# install googletest
wget https://github.com/google/googletest/archive/release-1.8.0.tar.gz && \
tar zxf release-1.8.0.tar.gz && \
rm -f release-1.8.0.tar.gz && \
cd googletest-release-1.8.0 && \
mkdir _build && cd _build && \
cmake .. && \
make -j "$(nproc)" && \
make install && \
cd ../..

# install folly
git clone https://github.com/facebook/folly && \
cd folly && \
mkdir _build && cd _build && \
cmake .. && \
make -j "$(nproc)" && \
make install && \
cd .. && \
./build.sh

# install Qt
apt-get install -y \
    qt5-default \
    qtmultimedia5-dev \
    libqt5multimedia5-plugins

# install boost & ZMQ
apt-get install -y \
    libboost-all-dev \
    libzmq3-dev
