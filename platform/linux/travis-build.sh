#!/bin/sh

if [ "$BUILD" = "gcc" ]; then
	export CXX=g++
	make -j2
fi

if [ "$BUILD" = "clang" ]; then
	export CXX=clang++-3.6
	make -j2
fi

if [ "$BUILD" = "analyze" ]; then
	export CXX=clang++-3.6
	scan-build-3.6 make -j2
fi

export LD_LIBRARY_PATH=/var/tmp/travis/lib
bin/raycer
