#!/bin/sh
[ ! -d "build" ] && meson setup build/ --prefix=/usr
ninja -C build/
