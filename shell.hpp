#pragma once

#include <iostream>

#include "var.hpp"
#include "exec.hpp"
#include "preprocessor.hpp"
#include "lib/lib_style.hpp"
#include "lib/lib_string.hpp"

namespace shell {
    void prep_for_shell(std::string& shell_cmd) {
        preproc::remove_comments(shell_cmd);
        shell_cmd = lib::trim_leading(shell_cmd);
        shell_cmd = lib::trim_trailing(shell_cmd);
    }

    void init_shell() {
        int count = 1;
        //VarTable var = VarTable();

        while(true) {
            std::string command;
            std::vector<std::string> prog_args = {};
            std::vector<Token> tokens;

            std::cout << style::style["bold"] << "\nKal" << style::style["reset"] << " " << style::style["bold"] << style::style["green"] << "[" << count << "]:" << style::style["reset"] << " ";
            std::getline(std::cin, command);
            if(std::cin.eof()) {
                std::cout << "\n" << std::endl;
                exit(0);
            }

            prep_for_shell(command);

            if(command == "") {
                continue;
            }

            if(command == ".reset") {
                VarTable::gc();
                continue;
            }

            if(command == ".multi") {
                std::string multi_prompt = style::style["bold"] + style::style["blue"] + "> " + style::style["reset"];
                std::string multi_line;
                std::vector<std::string> multi_lines;
                std::cout << style::style["bold"] << style::style["blue"] << "\nIn:" << style::style["reset"] << "\n";
                std::cout << multi_prompt;
                while(std::getline(std::cin, multi_line) && multi_line != ".eof") {
                    prep_for_shell(multi_line);
                    std::cout << multi_prompt;
                    if(multi_line != "") {
                        multi_lines.emplace_back(multi_line);
                        count++;
                    }
                }
                if(multi_lines.size() != 0) {
                    tokens = lexer::tokenize(multi_lines);
                    std::cout << style::style["bold"] << style::style["blue"] << "\nOut:" << style::style["reset"] << "\n";
                    line_exec(tokens);
                }
                continue;
            }

            if(command[0] == '@') {
                std::string file_name = command.substr(1);
                std::vector<std::string> preprocessed_lines = preproc::preprocess_file(file_name);
                tokens = lexer::tokenize(preprocessed_lines);
                line_exec(tokens);
                count++;
                continue;
            }

            std::vector<std::string> shell_lines = lib::split(command);
            for(std::string& each : shell_lines) {
                prep_for_shell(each);
            }
            tokens = lexer::tokenize(shell_lines);
            line_exec(tokens);

            count++;
        }
    }
}
