#!/usr/bin/env bash

compiler="clang++"
os=$(uname)

if [[ "$os" == 'Linux' ]]; then
	compiler="g++"
fi

if ! [[ -z "${COMPILER}" ]]; then
	compiler="${COMPILER}"
fi

compiler_version=$($compiler --version)
echo "Using compiler: $compiler, version: $compiler_version"

echo "Creating directories ..."

mkdir -p "buildfiles/src/Logger"
mkdir -p "buildfiles/src/Project"
mkdir -p "bin"

find src -name "*.cpp" | grep -v "tests" | grep -v "main.cpp" | while read -r src_file; do
	echo "Compiling: $src_file ..."
	$compiler -O2 -std=c++14 -c $src_file -o buildfiles/$src_file.o -I/usr/local/include
	if ! [[ $? == 0 ]]; then
		break
	fi
done

if ! [[ $? == 0 ]]; then
	echo "Error in compiling sources, will not continue"
	exit $?
fi

echo "Building ..."

buildfiles=$(find buildfiles -name "*.cpp.o" | paste -sd " " -)
$compiler -O2 -std=c++14 -g -o bin/ccp4m src/main.cpp $buildfiles -I/usr/local/include -L/usr/local/lib -lyaml-cpp -lcurl -lpthread
