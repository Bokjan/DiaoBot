#!/bin/bash

set -o errexit

cd `dirname $0`
SCRIPT_PATH=`pwd`/
CORE_COUNT=`grep 'processor' /proc/cpuinfo | sort -u | wc -l`

mkdir -p SDK/build
mkdir -p Daemon/build

cd SDK/build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE
make -j${CORE_COUNT}
sudo make install
# cp libdiaobot.so ../../Dist

cd ../../Daemon/build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE
make -j${CORE_COUNT}
cp diaobotd ../../Dist