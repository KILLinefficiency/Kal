#pragma once

#include "config.hpp"
#include "lexer.hpp"
#include "errors.hpp"
#include "expr_parser.hpp"
#include "var.hpp"
#include "lib/lib_style.hpp"
#include "lib/lib_string.hpp"
//#include "lib/lib_list.hpp"

namespace parser {
    void std_out(std::string out_text) {
        if(out_text[0] == '$') {
            std::cout << lib::resolve_string(VarTable::print(out_text));
            return;
        }
        std::cout << lib::resolve_string(lib::render_escape_chars(out_text));
    }

    void std_err(std::string err_text) {
        std::cerr << lib::resolve_string(lib::render_escape_chars(err_text));
    }
}

void line_exec(std::vector<Token>& tokens) {
    bool warn = true;
    int total_tokens = tokens.size();

    int line = 0;
    int depth = 0;
    int current_depth = 0;
    while(line < total_tokens) {
        Token& cmd = tokens[line];
        std::string& ins = cmd.head;
        int cmd_size = cmd.values.size();
        if(ins[0] == '#' && ins[1] == '!') {
            line++;
            continue;
        }

        if(tokens[line].head == "if" && tokens[line].values[tokens[line].values.size() - 1] == "{") {
            depth += 1;
            current_depth = depth;
            // might need to refactor values into a variable.
            bool condition = eval(tokens[line].values[0]) == "1";
            if(condition) {
                line++;
                continue;
            }
            else {
                while(depth != current_depth - 1) {
                    line++;
                    if(tokens[line].values.size() != 0 && tokens[line].values[tokens[line].values.size() - 1] == "{") { depth++; }
                    if(tokens[line].head == "}") { depth--; }
                }
            }
        }
        if(tokens[line].head == "}") { depth--; }

        else if(ins == "exit") {
            if(cmd_size == 0) {
                exit(0);
            }
            else {
                int exit_code = stoi(cmd.values[1]);
                exit(exit_code);
            }
        }

        else if(ins == "warn") {
            if(cmd_size == 0) {
                warn = !warn;
            }
            else if(cmd_size == 1) {
                if(cmd.values[0] == "off") {
                    warn = false;
                }
                else if(cmd.values[0] == "on") {
                    warn = true;
                }
            }
        }

        else if(ins == "style") {
            for(int style_itr = 0; style_itr < cmd_size; style_itr++) {
                std::string passed_style = cmd.values[style_itr];
                if(passed_style[0] == '"' && passed_style[passed_style.size() - 1] == '"') {
                    passed_style = passed_style.substr(1, passed_style.size() - 2);
                }
                std::string current_style = style::style[passed_style];

                if(current_style != "") {
                    std::cout << current_style;
                }
            }
        }

        else if(ins == "var" || (ins == "" && cmd.init.size() != 0)) {
            int vars = cmd.init.size();
            for(int each = 0; each < vars; each += 2) {
                VarTable::set(cmd.init[each], cmd.init[each + 1]);
            }
        }

        else if(ins == "stdout") {
            if(cmd_size == 0) {
                std::cout << "";
                line++;
                continue;
            }

            for(int start_val = 0; start_val < cmd_size; start_val++) {
                parser::std_out(cmd.values[start_val]);
            }
        }

        /*else if(ins == "stdin" && cmd_size == 1) {
            std::string var_to_read = lexer::get_var_name_from_token(cmd.values[0]);
            if(var.get_mem_type(var_to_read) == "const") {
                errors::change_const_var_error(var_to_read);
            }
            //var.read_var(var.expand_var(cmd[1]));
            var.read_var(cmd.values[0]);
        }*/

        line++;
    }

    std::cout << style::style["reset"];
}