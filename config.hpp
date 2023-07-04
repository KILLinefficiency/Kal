#pragma once

#include <vector>

struct Token {
    std::string head = "", target ="";
    std::vector<std::string> init = {}, values = {};
};

struct Config {
    std::string id = "";
    bool head           =  true,
         optional_arg   =  true,
         single_arg     =  false,
         multiple_args  =  false,
         init_list      =  false,
         target         =  false;
};


Config group_0 = {
    .head = false,
    .init_list = true,
};

Config group_1;

Config group_2 = {
    .optional_arg = false,
    .single_arg = true,
};

Config group_3 = {
    .multiple_args = true,
};

Config group_4 = {
    .id = "group_4",
    .init_list = true,
};

Config group_5 = {
    .multiple_args = true,
    .target = true,
};

namespace p_config {
    Config* get_config(const std::string& cmd) {
        if(cmd == "exit" || cmd == "warn") {
            return &group_1;
        }
        else if(cmd == "stdin") {
            return &group_2;
        }
        else if(cmd == "style" || cmd == "stdout") {
            return &group_3;
        }
        else if(cmd == "var" || cmd == "const" || cmd == "static") {
            return &group_4;
        }

        return &group_0;
    }
}