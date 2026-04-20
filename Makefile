EXECUTABLE := DemoCrocobyGraph
GENERATOR := Ninja
.DEFAULT_GOAL := all
.SILENT: run
.PHONY: clear configure_debug build_debug debug release run

export CC := /usr/bin/clang
export CXX := /usr/bin/clang++

all: configure_debug build_debug run

clear:
	rm -rf ./build

configure_debug:
	cmake -S. -B=build -G=${GENERATOR} -DCMAKE_BUILD_TYPE=Debug

build_debug:
	cmake --build build -j 8 --config Debug

configure_release:
	cmake -S. -B=build -G=${GENERATOR} -DCMAKE_BUILD_TYPE=Release

build_release:
	cmake --build build -j 8 --config Release

debug: configure_debug build_debug
release: configure_release build_release

run:
	echo
	echo ---- PROGRAM OUTPUT: ----
	./build/${EXECUTABLE}
