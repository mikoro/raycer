#!/bin/sh

if [ "$CXX" == "clang++" ]; then
	scan-build make
else
	make
fi

export LD_LIBRARY_PATH=/var/tmp/travis/lib
bin/raycer
