#!/bin/bash
CURRENT_DIR=$(cd $(dirname -- "$0"); pwd)
export AD_HOME=$CURRENT_DIR
export AD_DATA=$CURRENT_DIR
export AD_LOG=$CURRENT_DIR

export LD_LIBRARY_PATH=$AD_HOME/lib:$LD_LIBRARY_PATH