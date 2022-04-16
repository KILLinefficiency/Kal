#!/usr/bin/env bash

function build() {
    g++ kal.cpp -o kal
}

[ "$1" == "build" ] && build
