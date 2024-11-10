#!/bin/bash

# Define the build directory
BUILD_DIR="build"
LIB_DIR="lib"

# Check if the build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating $BUILD_DIR directory..."
    mkdir "$BUILD_DIR"
fi

# Check if the build directory exists
if [ ! -d "$LIB_DIR" ]; then
    echo "Creating $LIB_DIR directory..."
    mkdir "$LIB_DIR"
fi

cd "$BUILD_DIR" || exit

cmake ..

make

echo
echo "finished build"
echo

for test_executable in test*; do
    # Check if it's an executable file
    if [ -x "$test_executable" ] && [ ! -d "$test_executable" ]; then
        echo "Running $test_executable..."
        echo "----------------------------------------------------"
        echo
        ./"$test_executable"
        echo
    fi
done