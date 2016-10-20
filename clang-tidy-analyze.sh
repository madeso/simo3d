#!/bin/bash

if [ ! -d "build/" ]; then
	echo "error: build the project first"
	exit 1
fi

clang-tidy -checks="*,-llvm*,-boost*,-cppcoreguidelines-pro-type-union-access" -p=build src/data.cc

