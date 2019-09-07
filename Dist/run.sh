#!/bin/bash

set -o errexit

cd `dirname $0`
SCRIPT_PATH=`pwd`/
# echo $SCRIPT_PATH

export LD_LIBRARY_PATH=$SCRIPT_PATH:$LD_LIBRARY_PATH

./diaobot.out "${SCRIPT_PATH}sample.conf"