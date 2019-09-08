#!/bin/bash

set -o errexit

cd `dirname $0`
SCRIPT_PATH=`pwd`/

#THIRD_BUILDS_PREFIX="/data/builds/"

mkdir -p SDK/build
mkdir -p Daemon/build

cd SDK/build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE
make -j7
make install
# cp libdiaobot.so ../../Dist

cd ../../Daemon/build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE #-DTHIRD_BUILDS_PREFIX="/data/builds/"
make -j7
cp diaobot.out ../../Dist