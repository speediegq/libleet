if (-Not (Test-Path "build")) {
    meson setup build
}

ninja -C build/
