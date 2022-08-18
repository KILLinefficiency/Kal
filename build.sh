#!/usr/bin/env bash

DEFAULT="\e[00;39m"
DEFAULT_BOLD="\e[01;39m"
GREEN="\e[01;32m"

CC="g++"
SRC_FILE="kal.cpp"
BIN_FILE="bin/kal-$(echo $(uname) | tr A-Z a-z)-$(uname -m)"
OPTIMIZATION="-O2"
FLAGS="-s -pipe -Wall -static -Werror -pedantic -fstack-protector"

SU="sudo"
[ $(whoami) == "root" ] && SU=""

function vim_ft() {
    echo -e "${GREEN} * Installing Kal Filetype for Vim${DEFAULT}"
    ! [ -d ~/.vim/syntax ] && mkdir -p ~/.vim/syntax
    cp syntax/kal.vim ~/.vim/syntax
    ft_exists=$(grep "kal" ~/.vimrc)
    [ -z "$ft_exists" ] && echo -e "\nau BufRead,BufNewFile *.kal set filetype=kal" >> ~/.vimrc
}

function compile() {
    echo -e "${GREEN} * Compiling Kal${DEFAULT}"
    ! [ -d bin ] && mkdir bin
    ${CC} ${OPTIMIZATION} ${FLAGS} ${SRC_FILE} -o ${BIN_FILE}
}

function install() {
    compile
    echo -e "${GREEN} * Installing Kal to /usr/local/bin/${DEFAULT}"
    ${SU} cp $BIN_FILE /usr/local/bin/kal
    vim_ft
}

function docker_run() {
    docker build -t kal .
    clear
    echo -e "${DEFAULT_BOLD} * Kal Docker Container running...${DEFAULT}"
    docker run -it --rm --name Kal kal
}

[ "$1" == "vim" ] && vim_ft
[ "$1" == "docker" ] && docker_run
[ "$1" == "compile" ] && compile
[ "$1" == "install" ] && install
