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

    std::string* line;
};

struct Config {
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

/*
 * Defines an instruction with head and multiple arguments and no target operator.
 * Format: ins arg1 arg2 arg3 ...
 */
Config group_8 = {
    .id = 8,
    .head = true,
    .multiple_args = true,
    .target = false,
};

namespace p_config {
    std::unordered_map<std::string, Config*> all_config = {
        { "exit", &group_1 },
        { "warn", &group_1 },
        { "break", &group_1 },
        { "continue", &group_1 },

        { "var", &group_2 },
        { "const", &group_2 },
        { "static", &group_2 },
        { "inert", &group_2 },

        { "stdin", &group_3 },
        { "style", &group_3 },
        { "type", &group_3 },
        { "push", &group_3 },
        { "len", &group_3 },
        { "first", &group_3 },
        { "last", &group_3 },
        { "pop", &group_3 },
        { "popFirst", &group_3 },
        { "reverse", &group_3 },
        { "shuffle", &group_3 },
        { "extend", &group_3 },
        { "remove", &group_3 },
        { "rotate", &group_3 },
        { "flat", &group_3 },
        { "slice", &group_3 },
        { "keys", &group_3 },
        { "values", &group_3 },
        { "items", &group_3 },
        { "update", &group_3 },
        { "exists", &group_3 },
        { "binWrite", &group_3 },
        { "binRead", &group_3 },
        { "rand", &group_3 },
        { "time", &group_3 },

        { "if", &group_4 },
        { "else", &group_4 },
        { "elif", &group_4 },
        { "loop", &group_4 },
        { "}", &group_4 },

        { "fn", &group_5 },
        { "<-", &group_6 },
        { "{", &group_7 },

        { "stdout", &group_8 },
        { "stderr", &group_8 },
        { "defer", &group_8 },
    };

    Config* get_config(std::string& line, std::string& cmd/*, Globals& globals*/) {
        if(false) {
            std::cout << line << "\n";
        }

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

        errors::unidentified_keyword(line);
        return &group_0;
    }
}
