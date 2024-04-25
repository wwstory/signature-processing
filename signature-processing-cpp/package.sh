#!/bin/sh

# on debian
# apt install build-essential cmake make \
#     libopencv-core-dev libopencv-imgproc-dev libopencv-dev \
#     libfmt-dev

# on centos
# yum install gtk2-devel

rm -rf build/ out/
# cmake -B build/ -DCMAKE_BUILD_TYPE=Debug . && \
cmake -B build/ -DCMAKE_BUILD_TYPE=Release . && \
cmake --build build

mkdir out
mkdir out/lib
cp ./build/libsignature_processing.so out/
ldd build/libsignature_processing.so | awk -F"=>" '/opencv/{print $2}' | awk '{print $1}' | xargs -i cp -rL {} out/lib/

# ldd build/libsignature_processing.so | awk -F"=>" '{print $2}' | awk '{print $1}' | xargs -i cp -r {} out/lib/
# ldd build/libsignature_processing.so | awk -F"=>" '{print $2}' | awk '{print $1}' | xargs -i cp -rL {} out/lib/
# cd out/lib && rm -rf linux-vdso.so.* libstdc++.so.* libc.so.* libm.so.* libgcc_s.so.* libpthread.so.*

# server loop
# ldd ../libsignature_processing.so | awk -F"=>" '/not found/{print $1}' | awk '{print $1}' | xargs -i cp -r {} ../lib

cp ./build/test_signature_processing out/
mv out/ sign/
mkdir out/; mv sign/ out/
cd out/
tar cvf sign.tar sign
