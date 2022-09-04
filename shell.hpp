#pragma once

#include <iostream>

#include "parser.hpp"
#include "preprocessor.hpp"
#include "lib/lib_style.hpp"
#include "lib/lib_string.hpp"

namespace shell {
    void init_shell() {
        int count = 1;
        VarTable var = VarTable();
        std::string init_file_path = "";

        while(true) {
            std::string command;
            std::vector<std::string> prog_args = {};
            std::vector<std::vector<std::string>> tokens;

            std::cout << style::bold << "\nKal" << style::reset << " " << style::bold << style::green << "[" << count << "]:" << style::reset << " ";
            std::getline(std::cin, command);
            if(std::cin.eof()) {
                std::cout << "\n" << std::endl;
                exit(0);
            }

            command = preproc::remove_comments(command);
            command = lib::trim_leading(command);
            command = lib::trim_trailing(command);

            if(command == "") {
                continue;
            }

            if(command == ".reset") {
                var = VarTable();
                continue;
            }

            if(command == ".multi") {
                std::string multi_prompt = style::bold + style::blue + "> " + style::reset;
                std::string edit_line;
                std::vector<std::string> edit_lines;
                std::cout << style::bold << style::blue << "\nIn:" << style::reset << "\n";
                std::cout << multi_prompt;
                while(std::getline(std::cin, edit_line) && edit_line != ".eof") {
                    std::cout << multi_prompt;
                    if(edit_line != "") {
                        edit_lines.emplace_back(edit_line);
                        count++;
                    }
                }

                tokens = lexer::tokenize(edit_lines);
                std::cout << style::bold << style::blue << "\nOut:" << style::reset << "\n";
                line_exec(tokens, var, prog_args);
                continue;
            }

            if(command[0] == '@') {
                std::string file_name = command.substr(1);
                std::vector<std::string> source_lines = preproc::initial_preprocessing(file_name);
                preproc::preprocess(source_lines, init_file_path);
                tokens = lexer::tokenize(source_lines);
                line_exec(tokens, var, prog_args);
                count++;
                continue;
            }

            tokens = { lib::split(command, ' ') };
            line_exec(tokens, var, prog_args);

            count++;
        }
    }
}
