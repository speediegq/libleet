# libleet Makefile
# ================
#
# This is a makefile which exists solely because some build tools in the year 2024 do not support Meson.
# As such, this acts as a wrapper for some of Meson's most basic features.
# If building manually, prefer Meson and Ninja. This Makefile might be removed in the future.

ninja:
	[ ! -d "build" ] && meson setup build || :
	ninja -C build

install: ninja
	meson install -C build/

install_doas: ninja
	doas meson install -C build/

install_sudo: ninja
	sudo meson install -C build/

clean:
	[ -d "build" ] && rm -rf build/ || :
