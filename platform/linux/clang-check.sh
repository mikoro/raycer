#!/bin/sh

find src -name '*.cpp' | xargs -I file --verbose clang-check -analyze -fixit file -- -isystem include -Isrc -std=c++14 -Wpedantic -Wall -Wextra -Werror -Ofast -Wno-deprecated-declarations
