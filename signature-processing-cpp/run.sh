#!/bin/sh

rm -rf build/
cmake -B build/ -DCMAKE_BUILD_TYPE=Debug . && \
# cmake -B build/ -DCMAKE_BUILD_TYPE=Release . && \
cmake --build build && \
cd build;
./test_signature_processing ../../datasets/test.jpg
