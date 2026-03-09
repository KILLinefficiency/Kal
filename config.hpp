#pragma once

#include <vector>
#include <stack>
#include <utility>

#include "errors.hpp"

std::stack<std::pair<std::string, int>> call_stack;

struct Token {
    std::string head = "",
                target = "";

    std::vector<std::string> init = {},
                             values = {};
};

struct Config {
    //std::string id = "";
    int id = 0;
    bool head           =  false,
         optional_arg   =  false,
         single_arg     =  false,
         multiple_args  =  false,
         init_list      =  false,
         target         =  false,
         tri            =  false;
};

/*
 * Defines initialization list for re-assignment of values.
 * Format: $var1 = val1, $var2 = val3, $var3 = $var1, ...
 */
Config group_0 = {
    .id = 0,
    .init_list = true,
};

/*
 * Defines an instruction with head which takes in one or no argument.
 * Format: ins
 *         ins $var
 */
Config group_1 = {
    .id = 1,
    .head = true,
    .optional_arg = true,
    .single_arg = true,
};


/*
 * Defines an head with initialization list for variable assignment and strictly does not have a target.
 * Format: const var1 = val1, var2 = val2, var3 = val3, ...
 */
Config group_2 = {
    .id = 2,
    .head = true,
    .init_list = true,
};

/*
 * Defines an instruction with head and multiple arguments with optional target operator. 
 * Format: ins arg1 arg2 arg3 ...
 *         ins arg1 arg2 arg3 ... -> $target
 */
Config group_3 = {
    .id = 3,
    .head = true,
    .multiple_args = true,
    .target = true,
};

/*
 * Defines an "if" statement.
 */

Config group_4 = {
    .id = 4,
    .tri = true,
};

/* Defines a named or an anonymous function.
 * Format: fn fn_name -> fn_arg1 fn_arg2 ...
 *         fn -> fn_arg1 fn_arg2 ...
 */
Config group_5 = {
    .id = 5,
    .head = true
};

/*
 * Defines a return statement.
 * Format: <- expression
 */
Config group_6 {
    .id = 6,
    .head = true,
    .single_arg = true,
};

Config group_7 {
    .id = 7,
    .head = true,
};

namespace p_config {
    std::unordered_map<std::string, Config*> all_config = {
        { "exit", &group_1 },
        { "warn", &group_1 },
        { "stdin", &group_1 },
        { "break", &group_1 },
        { "continue", &group_1 },

        { "var", &group_2 },
        { "const", &group_2 },
        { "static", &group_2 },
        { "inert", &group_2 },

        { "style", &group_3 },
        { "stdout", &group_3 },
        { "defer", &group_3 },
        { "push", &group_3 },
        { "len", &group_3 },
        { "first", &group_3 },
        { "last", &group_3 },

        { "if", &group_4 },
        { "else", &group_4 },
        { "elif", &group_4 },
        { "loop", &group_4 },
        { "}", &group_4 },

        { "fn", &group_5 },
        { "<-", &group_6 },
        { "{", &group_7 },
    };

    Config* get_config(std::string& line, std::string& cmd) {
        // if(cmd == "exit" || cmd == "warn" || cmd == "stdin" || cmd == "break" || cmd == "continue"/* || cmd == "<-"*/) {
        //     return &group_1;
        // }
        // else if(cmd == "var" || cmd == "const" || cmd == "static" || cmd == "inert") {
        //     return &group_2;
        // }
        // else if(cmd[0] == ':' || cmd == "style" || cmd == "stdout" || cmd == "defer" || cmd == "push" || cmd == "len") {
        //     return &group_3;
        // }
        // else if(cmd == "if" || cmd == "else" || cmd == "elif" || cmd == "loop" || cmd == "}") {
        //     return &group_4;
        // }
        // else if(cmd == "fn") {
        //     return &group_5;
        // }
        // else if(cmd == "<-") {
        //     return &group_6;
        // }
        // else if(cmd == "{") {
        //     return &group_7;
        // }
        // else {
        //     return &group_0;
        // }
        /*else if(cmd[0] == '$') {
            return &group_0;
        }*/
        if(cmd[0] == ':') {
            return &group_3;
        }
        else {
            Config* config = all_config[cmd];
            if(config != nullptr) {
                return config;
            }
            return &group_0;
        }

        errors::unidentified_keyword(call_stack, line, cmd);
        return &group_0;
    }
}