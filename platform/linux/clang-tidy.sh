#!/bin/sh

CHECKS="*"

find src -name '*.cpp' | xargs -I file --verbose clang-tidy -checks=$CHECKS file -- -isystem include -Isrc -std=c++14 -Wpedantic -Wall -Wextra -Werror -Ofast -Wno-deprecated-declarations
