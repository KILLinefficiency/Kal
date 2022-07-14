#pragma once

#include <iostream>

#include "parser.hpp"
#include "preprocessor.hpp"
#include "lib/lib_style.hpp"

namespace shell {
    void init_shell() {
        int count = 1;
        VarTable var = VarTable();

        while(true) {
            std::string command;
            std::cout << style::bold << "\nKal" << style::reset << " " << style::bold << style::green << "[" << count << "]:" << style::reset << " ";
            std::getline(std::cin, command);
            command = preproc::remove_comments(command);

            if(command == "") {
                continue;
            }

            std::vector<std::vector<std::string>> tokens = {lib::split(command, ' ')};
            line_exec(tokens, var);

            count++;
        }
    }
}
