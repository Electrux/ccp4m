#!/usr/bin/env bash

compiler="clang++"
os=`uname`

if [[ "$os" == 'Linux' ]]; then
	compiler="g++"
fi

if ! [[ -z "${COMPILER}" ]]; then
	compiler="${COMPILER}"
fi

compiler_version=`$compiler --version`
echo "Using compiler: $compiler, version: $compiler_version"

echo "Creating directories ..."

mkdir -p "buildfiles/src/Logger"
mkdir -p "buildfiles/src/Project"
mkdir -p "bin"

find src -name "*.cpp" | grep -v "tests" | grep -v "main.cpp" | while read -r src; do
	echo "Compiling: $compiler -O2 -std=c++14 -c $src -o buildfiles/$src.o"
	$compiler -O2 -std=c++11 -c $src -o buildfiles/$src.o
	if ! [[ $? == 0 ]]; then
		break
	fi
done

if ! [[ $? == 0 ]]; then
	exit $?
fi

echo "Building ..."

buildfiles=`find buildfiles -name "*.cpp.o" | paste -sd " " -`
$compiler -O2 -std=c++14 -g -o bin/ccp4m src/main.cpp $buildfiles -lyaml-cpp -lcurl -lpthread