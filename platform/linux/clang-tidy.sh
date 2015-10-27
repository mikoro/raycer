#!/bin/sh

CHECKS="*,\
-google-build-using-namespace,\
-google-readability-namespace-comments,\
-google-readability-braces-around-statements,\
-llvm-include-order,\
-readability-braces-around-statements,\
-google-readability-todo,\
-llvm-namespace-comment,\
-readability-else-after-return,\
"

find src -name '*.cpp' | xargs -I file --verbose clang-tidy -checks=$CHECKS file -- -isystem include -Isrc -std=c++14 -Wpedantic -Wall -Wextra -Werror -Ofast -Wno-deprecated-declarations
