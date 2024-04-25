#!/bin/sh

rm -rf build/
cmake -B build/ -DCMAKE_BUILD_TYPE=Debug . && \
# cmake -B build/ -DCMAKE_BUILD_TYPE=Release . && \
cmake --build build && \
cd build;
# ./test_signature_processing ../test/zhangxiaoyun.jpeg
# ./test_signature_processing ../test/blank.jpg
# ./test_signature_processing ../test/blank.png
# ./test_signature_processing ../test/input.bmp
./test_signature_processing ../test/202404290910-comp/EC2404190000223-20240419111951.jpg
