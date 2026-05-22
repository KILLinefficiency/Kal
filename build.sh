#!/usr/bin/env bash

DEFAULT="\e[00;39m"
DEFAULT_BOLD="\e[01;39m"
BLUE="\e[01;34m"
GREEN="\e[01;32m"

CC="g++"
SRC_FILE="kal.cpp"
BIN_FILE="bin/kal-$(echo $(uname) | tr A-Z a-z)-$(uname -m)"
O_FILE="bin/libkal.o"
SO_FILE="bin/libkal.so"
AR_FILE="bin/libkal.a"
OPTIMIZATION="-O2"
STD="-std=c++20"

LIBKAL_SRC="embed/libkal.cpp"

DYNAMIC_LIBKAL_SRC="embed/dynamic_libkal.cpp"
LIBKAL_DYNAMIC_TEMP_OBJ="bin/dynamic_libkal.o"
LIBKAL_SHARED_OBJ="bin/libkal.so"

STATIC_ELF=""
if [ -z $STATIC ]; then
    STATIC=0
fi
if [ $STATIC -eq 1 ]; then
    STATIC_ELF="-static"
fi

FLAGS="-pipe $(echo $STATIC_ELF) -pedantic -Wall -Wextra -Werror"
NO_DEBUG_INFO="-s"

TEST_SRC_FILE="tests/kal_test.cpp"
TEST_BIN_FILE="bin/kal_test"

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
    echo -e "    │ ${DEFAULT_BOLD}./build.sh test         ${GREEN}Runs Tests.                     │"
    echo -e "    │ ${DEFAULT_BOLD}./build.sh docker       ${GREEN}Runs Docker container for Kal.  │"
    echo -e "    │ ${DEFAULT_BOLD}./build.sh embed        ${GREEN}Compiles libkal library.        │"
    echo -e "    │ ${DEFAULT_BOLD}./build.sh help         ${GREEN}Displays this help message.     │"
    echo -e "    │                                                         │"
    echo -e "    └─────────────────────────────────────────────────────────┘${DEFAULT}\n"
}


function run_tests() {
    echo -e "${GREEN} * Compiling Kal Tests${DEFAULT}"
    ! [ -d bin ] && mkdir bin
    ${CC} ${OPTIMIZATION} ${STD} ${FLAGS} -ggdb3 ${TEST_SRC_FILE} -o ${TEST_BIN_FILE} && 
        echo -e "${GREEN} * Running Kal Tests${DEFAULT}" &&
        ${TEST_BIN_FILE}
}

function compile() {
    start=$(date +%s)

    echo -en "${GREEN} * Compiling Kal${BLUE}        "
    ! [ -d bin ] && mkdir bin
    ${CC} ${NO_DEBUG_INFO} ${OPTIMIZATION} ${STD} ${FLAGS} ${SRC_FILE} -o ${BIN_FILE}

    end=$(date +%s)
    echo -en "[" $((end - start)) "sec ]${DEFAULT}\n"
}

function embed() {
    start=$(date +%s)
    echo -en "${GREEN} * Compiling libkal.o${BLUE}   "
    ! [ -d bin ] && mkdir bin
    cp ./embed/kal.hpp ./bin/
    ${CC} -c -s ${NO_DEBUG_INFO} ${OPTIMIZATION} ${STD} ${FLAGS} ${LIBKAL_SRC} -o ${O_FILE}
    end=$(date +%s)
    echo -en "[" $((end - start)) "sec ]${DEFAULT}\n"

    start=$(date +%s)
    echo -en "${GREEN} * Compiling libkal.a${BLUE}   "
    ar crf ${AR_FILE} ${O_FILE}
    end=$(date +%s)
    echo -en "[" $((end - start)) "sec ]${DEFAULT}\n"

    start=$(date +%s)
    echo -en "${GREEN} * Compiling libkal.so${BLUE}  "
    ${CC} -c -s -fPIC ${NO_DEBUG_INFO} ${OPTIMIZATION} ${STD} ${FLAGS} ${DYNAMIC_LIBKAL_SRC} -o ${LIBKAL_DYNAMIC_TEMP_OBJ}
    ${CC} -shared ${LIBKAL_DYNAMIC_TEMP_OBJ} -o ${LIBKAL_SHARED_OBJ}
    rm ${LIBKAL_DYNAMIC_TEMP_OBJ}
    end=$(date +%s)
    echo -en "[" $((end - start)) "sec ]${DEFAULT}\n"
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

function install() {
    compile
    embed
    echo -e "${GREEN} * Installing Kal to /usr/local/bin/${DEFAULT}"
    ${SU} cp $BIN_FILE /usr/local/bin/kal
    
    for version in /usr/include/c++/*; do
        if [ -d $version ]; then
            ${SU} cp ./bin/kal.hpp ${version}/kal
        fi
    done

    echo -e "${GREEN} * Installing libkal.o to /usr/local/lib/${DEFAULT}"
    ${SU} cp $O_FILE /usr/local/lib/libkal.o

    echo -e "${GREEN} * Installing libkal.so to /usr/local/lib/${DEFAULT}"
    ${SU} cp $SO_FILE /usr/local/lib/libkal.so

    echo -e "${GREEN} * Installing libkal.a to /usr/local/lib/${DEFAULT}"
    ${SU} cp $AR_FILE /usr/local/lib/libkal.a
    vim_ft
}

function docker_run() {
    docker build -t kal .
    clear
    echo -e "${DEFAULT_BOLD} * Kal Docker Container running...${DEFAULT}"
    docker run -it --rm --name Kal kal
}

rm -f core.*

if [ -z "$1" ]; then
    get_help
elif [ "$1" == "docker" ]; then
    docker_run
elif [ "$1" == "compile" ]; then
    compile
elif [ "$1" == "install" ]; then
    install
elif [ "$1" == "test" ]; then
    run_tests
elif [ "$1" == "embed" ]; then
    embed
else
    get_help
fi
