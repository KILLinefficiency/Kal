#pragma once

#include <vector>
#include <utility>
#include "parser.hpp"

#include "lib/lib_string.hpp"

namespace lexer {
    std::vector<Token> tokenize(const std::vector<std::string>& source_lines) {
        int lines = source_lines.size();
        /*for(auto y : source_lines) {
            std::cout << y << std::endl;
        }*/
        std::string head;
        Config* config;
        std::vector<Token> all_tokens;

        for(int line = 0; line < lines; line++) {
            int get_head = 0;
            std::string current_line = source_lines[line];
            if(current_line.substr(0, 2) == "if") {
                head = "if";
            }
            else if(current_line.substr(0, 4) == "else") {
                head = "else";
            }
            else if(current_line.substr(0, 4) == "loop") {
                head = "loop";
            }
            else {
                while(current_line[get_head] != ' ' && current_line[get_head] != '\t' && current_line[get_head] != '\n') {
                    get_head++;
                }
                head = current_line.substr(0, get_head);
            }
            config = p_config::get_config(head);
            Token token = parser::parse(current_line, config, head);
            all_tokens.emplace_back(token);
        }
        //exit(1);
        return all_tokens;
    }
}
