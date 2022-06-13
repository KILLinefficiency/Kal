#!/usr/bin/env bash

CC="g++"
SRC_FILE="kal.cpp"
BIN_FILE="bin/kal-$(uname)-$(uname -m)"
OPTIMIZATION="-O2"
FLAGS="-s -pipe -Wall -Werror -pedantic -fstack-protector"

SU="sudo"

function build() {
    ! [ -d bin ] && mkdir bin
    ${CC} ${OPTIMIZATION} ${FLAGS} ${SRC_FILE} -o ${BIN_FILE}
}

function install() {
    build
    ${SU} cp bin/kal-$(uname)-$(uname -m) /usr/local/bin/kal
}

[ "$1" == "compile" ] && build
[ "$1" == "install" ] && install
