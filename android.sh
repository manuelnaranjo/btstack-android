#!/bin/bash

rm -rf android
mkdir android

make clean
./configure --host=arm-linux-androideabi \
    --with-uart-device=/dev/ttyHS0 \
    --enable-powermanagement \
    --disable-bluetool \
    --enable-android
make

cp src/BTdaemon android
cp src/lib*.so android
cp $(find example -maxdepth 1 -type f -executable) android
adb push android /data/local/btstack
