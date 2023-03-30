#!/bin/sh

SOURCE_DIR=`pwd`

mkdir -p build && cd build && cmake -D CMAKE_INSTALL_PREFIX=. $SOURCE_DIR && make  && make install
