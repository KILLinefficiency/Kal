#!/usr/bin/env bash

CC="g++"
SRC_FILE="kal.cpp"
BIN_FILE="bin/kal-$(echo $(uname) | tr L l)-$(uname -m)"
OPTIMIZATION="-O2"
FLAGS="-s -pipe -Wall -static -Werror -pedantic -fstack-protector"

SU="sudo"

function build() {
    ! [ -d bin ] && mkdir bin
    ${CC} ${OPTIMIZATION} ${FLAGS} ${SRC_FILE} -o ${BIN_FILE}
}

function install() {
    build
    ${SU} cp $BIN_FILE /usr/local/bin/kal
}

function docker_run() {
    docker build -t kal .
    docker run -it --rm --name Kal kal
}

[ "$1" == "docker" ] && docker_run
[ "$1" == "compile" ] && build
[ "$1" == "install" ] && install
