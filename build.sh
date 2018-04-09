#!/bin/bash

compiler="clang++"
os=`uname`

if [[ "$os" == 'Linux' ]]; then
	compiler="g++"
fi

if ! [[ -z "${COMPILER}" ]]; then
	compiler="${COMPILER}"
fi

echo "Using compiler: $compiler"

echo "Creating directories ..."

mkdir -p "buildfiles/src/Logger"
mkdir -p "buildfiles/src/Project"
mkdir -p "bin"

find src -name "*.cpp" | grep -v "tests" | grep -v "main.cpp" | while read -r src; do
	echo "\033[0;33mCompiling: \033[0;32mbuildfiles/$src.o\033[0m"
	$compiler -O2 -std=c++14 -c $src -o buildfiles/$src.o
done

echo "\033[1;34mBuilding ..."

buildfiles=`find buildfiles -name "*.cpp.o" | paste -sd " " -`
$compiler -std=c++14 -lyaml-cpp -lcurl -lpthread -g -o bin/ccp4m src/main.cpp $buildfiles
