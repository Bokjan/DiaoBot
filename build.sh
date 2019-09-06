#!/bin/bash

set -o errexit

mkdir -p SDK/build
mkdir -p Daemon/build

cd SDK/build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE
make -j7

cd ../../Daemon/build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE
make -j7
cp diaobot.out ../../Dist