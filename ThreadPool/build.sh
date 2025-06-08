#!/bin/bash
CURRENT_DIR=$(cd $(dirname -- "$0"); pwd)
if [ -d $CURRENT_DIR/build ]; then
    echo "build exited"
else
    mkdir -p $CURRENT_DIR/build
fi
cd $CURRENT_DIR/build
cmake ..
make