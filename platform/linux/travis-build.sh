#!/bin/sh

if [ "$CXX" = "clang++" ]; then
	export CXX=clang++-3.6
	scan-build-3.6 make
else
	make
fi

export LD_LIBRARY_PATH=/var/tmp/travis/lib
bin/raycer
