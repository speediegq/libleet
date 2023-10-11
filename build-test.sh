#!/bin/sh
[ ! -d 'build' ] && meson setup build --prefix=/usr
cd build
meson install
cd ..

g++ test.cpp -o test -lleet && ./test
