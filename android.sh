#!/bin/bash

rm -rf android
mkdir android
cp src/BTdaemon android
cp src/lib*.so android
cp $(find example -maxdepth 1 -type f -executable) android
adb push android /data/local/btstack
