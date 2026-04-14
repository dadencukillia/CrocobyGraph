EXECUTABLE := DemoCrocobyGraph
GENERATOR := Ninja
.DEFAULT_GOAL := all
.SILENT: run

export CC := /usr/bin/clang
export CXX := /usr/bin/clang++

all: configure compile run

clear:
	rm -rf ./build

configure:
	cmake -S. -B=build -G=${GENERATOR}

compile:
	cmake --build build -j 8

run:
	echo
	echo ---- PROGRAM OUTPUT: ----
	./build/${EXECUTABLE}
