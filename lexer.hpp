#pragma once

#include <vector>
#include <utility>
#include <unordered_map>
#include "parser.hpp"

#include "types.hpp"
#include "lib/lib_string.hpp"

namespace Functions {
    std::unordered_map<std::string, Fn*> fn;

    void gc() {
        std::unordered_map<std::string, Fn*>::iterator itr, end = fn.end();
        for(itr = fn.begin(); itr != end; itr++) {
            if(itr->second != nullptr) {
                delete itr->second;
                fn[itr->first] = nullptr;
            }
        }
    }
}

namespace lexer {
    std::string get_head(std::string current_line) {
        std::string head = "";
        if(current_line.substr(0, 2) == "if") {
            head = "if";
        }
        else if(current_line.substr(0, 4) == "else") {
            head = "else";
        }
        else if(current_line.substr(0, 4) == "loop") {
            head = "loop";
        }
        else if(current_line.substr(0, 2) == "<-") {
            head = "<-";
        }
        else {
            int head_pos = 0;
            while(current_line[head_pos] != ' ' && current_line[head_pos] != '\t' && current_line[head_pos] != '\n') {
                head_pos++;
            }
            head = current_line.substr(0, head_pos);
        }
        return head;
    }

    std::vector<Token> tokenize(std::vector<std::string>& source_lines) {
        int lines = source_lines.size();
        Config* config;
        std::vector<Token> all_tokens;

        int line = 0;
        while(line < lines) {
            std::string current_line = source_lines[line];
            std::string head = get_head(current_line);
            config = p_config::get_config(head);
            Token token = parser::parse(current_line, config, head);
            if(token.head == "fn") {
                Fn* function = new Fn(token.values);
                int fn_depth = 1;
                Token fn_line;
                line++;
                while(fn_depth != 0) {
                    std::string inner_head = get_head(source_lines[line]);
                    Config* inner_config = p_config::get_config(inner_head);
                    fn_line = parser::parse(source_lines[line], inner_config, inner_head);
                    int values_size = fn_line.values.size();
                    if(values_size != 0 && fn_line.values[values_size - 1] == "{") {
                        fn_depth++;
                    }
                    if(fn_line.head == "}") {
                        fn_depth--;
                    }
                    if(fn_depth != 0 || fn_line.head != "}") {
                        function->body.emplace_back(fn_line);
                    }
                    line++;
                }
                Functions::fn[function->name] = function;
                //std::cout << "here: " << line << "\n";
                //line++;
                continue;
            }
            all_tokens.emplace_back(token);
            line++;
        }
        //exit(1);
        return all_tokens;
    }
}
