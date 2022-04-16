#!/usr/bin/env bash

function build() {
    ! [ -d bin ] && mkdir bin
    g++ kal.cpp -o bin/kal
}

function install() {
    build
    sudo cp bin/kal /usr/local/bin
}

[ "$1" == "build" ] && build
[ "$1" == "install" ] && install
