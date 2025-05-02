#!/bin/bash

# Install dependencies
apt update
apt install lsb-release clang wget curl software-properties-common gnupg pkg-config build-essential libssl-dev libelf-dev git vim llvm lld libelf-dev libzstd-dev libz-dev libcap-dev libbfd-dev

WS="$PWD"

# Clone bpftool
git clone https://github.com/libbpf/bpftool
pushd bpftool
git checkout c03ee3921594fd40b71cbfc17de66d414cd4d5bd

# Build libbpf
git submodule update --init
pushd libbpf/src
make install_headers
BUILD_STATIC_ONLY=y make -j
popd

# Build bpftool
pushd src
make -j
popd
popd

# Install SCHED
cd "$WS"
make -j