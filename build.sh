#!/usr/bin/env bash

DEFAULT="\e[00;39m"
DEFAULT_BOLD="\e[01;39m"
BLUE="\e[01;34m"
GREEN="\e[01;32m"

CC="g++"
SRC_FILE="kal.cpp"
BIN_FILE="bin/kal-$(echo $(uname) | tr A-Z a-z)-$(uname -m)"
OPTIMIZATION="-O2"
FLAGS="-s -pipe -static -pedantic -Wall -Wextra -Werror"

SU="sudo"
[ $(whoami) == "root" ] && SU=""

function get_help() {
    echo -e "${BLUE}"
    echo -e "    ┌─────────────────────────────────────────────────────────┐"
    echo -e "    │                     Kal Build Help                      │"
    echo -e "    ├─────────────────────────────────────────────────────────┤"
    echo -e "    ${GREEN}│                                                         │"
    echo -e "    │ ${DEFAULT_BOLD}./build.sh install      ${GREEN}Compiles and Installs Kal.      │"
    echo -e "    │ ${DEFAULT_BOLD}./build.sh compile      ${GREEN}Compiles Kal.                   │"
    echo -e "    │ ${DEFAULT_BOLD}./build.sh docker       ${GREEN}Runs Docker container for Kal.  │"
    echo -e "    │ ${DEFAULT_BOLD}./build.sh help         ${GREEN}Displays this help message.     │"
    echo -e "    │                                                         │"
    echo -e "    └─────────────────────────────────────────────────────────┘${DEFAULT}\n"
}

function vim_ft() {
    echo -e "${GREEN} * Installing Kal Filetype for Vim${DEFAULT}"
    ! [ -f ~/.vimrc ] && touch ~/.vimrc
    ! [ -d ~/.vim/syntax ] && mkdir -p ~/.vim/syntax
    cp syntax/kal.vim ~/.vim/syntax
    ft_exists=$(grep "kal" ~/.vimrc)
    [ -z "$ft_exists" ] && echo -e "\nsyntax on | au BufRead,BufNewFile *.kal set filetype=kal number autoindent noswapfile hlsearch incsearch tabstop=4 shiftwidth=4 expandtab clipboard=unnamedplus" >> ~/.vimrc
    return 0
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

if [ -z "$1" ]; then
    get_help
elif [ "$1" == "docker" ]; then
    docker_run
elif [ "$1" == "compile" ]; then
    compile
elif [ "$1" == "install" ]; then
    install
else
    get_help
fi
