#!/bin/sh

g++ -c -isystem include -Isrc -std=c++11 -Wpedantic -Wall -Wextra -Werror -Ofast -fopenmp -Wno-deprecated-declarations src/stdafx.h -o src/stdafx.h.gch
