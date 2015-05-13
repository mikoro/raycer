#!/bin/sh

if [ "$CXX" = "clang" ]
then
	scan-build make
else
	make
fi

bin/raycer
