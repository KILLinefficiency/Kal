#pragma once

#include <vector>

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

namespace p_config {
    Config* get_config(const std::string& cmd) {
        // refactor this into switch-case.
        if(cmd == "exit" || cmd == "warn" || cmd == "stdin" || cmd == "break" || cmd == "continue"/* || cmd == "<-"*/) {
            return &group_1;
        }
        else if(cmd == "var" || cmd == "const" || cmd == "static" || cmd == "inert") {
            return &group_2;
        }
        else if(cmd[0] == ':' || cmd == "style" || cmd == "stdout") {
            return &group_3;
        }
        else if(cmd == "if" || cmd == "else" || cmd == "loop" ||cmd == "}") {
            return &group_4;
        }
        else if(cmd == "fn") {
            return &group_5;
        }
        else if(cmd == "<-") {
            return &group_6;
        }
        else if(cmd[0] == '$') {
            return &group_0;
        }

        // throw error if no instruction is recognized.

        return &group_0;
    }
}