#pragma once

#include <vector>
#include <utility>
#include <unordered_map>
#include "parser.hpp"

#include "globals.hpp"
#include "types.hpp"
#include "lib/lib_string.hpp"

using FnTable = std::unordered_map<std::string, Fn*>;

namespace Functions {
    FnTable fn;

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
        int line_size = current_line.size();
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
            while(head_pos < line_size && current_line[head_pos] != ' ' && current_line[head_pos] != '\t' && current_line[head_pos] != '\n' && current_line[head_pos] != '=') {
                head_pos++;
            }
            head = current_line.substr(0, head_pos);
        }
        return head;
    }

    std::vector<Token> tokenize(std::vector<std::string>& source_lines, Globals& globals) {
        int lines = source_lines.size();
        Config* config;
        std::vector<Token> all_tokens;

        int line = 0;
        while(line < lines) {
            std::string current_line = source_lines[line];
            std::string head = get_head(current_line);
            config = p_config::get_config(current_line, head);
            Token token = parser::parse(current_line, config, head);
            token.line = &source_lines[line];

            if(token.head == "fn") {
                Fn* function = new Fn(token.values);
                int fn_depth = 1;
                Token fn_line;
                line++;
                while(fn_depth != 0) {
                    std::string inner_head = get_head(source_lines[line]);
                    Config* inner_config = p_config::get_config(source_lines[line], inner_head);
                    fn_line = parser::parse(source_lines[line], inner_config, inner_head);
                    fn_line.line = &source_lines[line];
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
                continue;
            }
            all_tokens.emplace_back(token);

            if((token.values.size() != 0 && token.values.back() == "{") || token.head == "{") {
                globals.jump_stack.push(line);
            }
            else if(token.head == "}") {
                if(!globals.jump_stack.empty()) {
                    uint64_t open = globals.jump_stack.top();
                    globals.jump_table[open] = line;
                    //globals.jump_table[line] = open;
                    globals.jump_stack.pop();
                }
            }

            line++;
        }

        JumpTable::iterator itr;
        for(itr = globals.jump_table.begin(); itr != globals.jump_table.end(); itr++) {
            std::cout << "[" << itr->first << ", " << itr->second << "]\n";
        }

        return all_tokens;
    }
}
