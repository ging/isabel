#! /bin/sh

export ISABEL_DIR=/usr/local/isabel

export ISABEL_CONFIG_DIR=$HOME/.isabel/config
export LD_LIBRARY_PATH=$ISABEL_DIR/lib

java -cp .:/usr/local/isabel/libexec/isabel_lib.jar LseServer

