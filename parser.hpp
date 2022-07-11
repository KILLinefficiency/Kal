#pragma once

#include "lexer.hpp"
#include "errors.hpp"
#include "variable.hpp"
#include "lib/lib_string.hpp"

namespace parser {
    void std_out(std::string out_text) {
        std::cout << lib::render_escape_chars(out_text);
    }

    void std_err(std::string err_text) {
        std::cerr << lib::render_escape_chars(err_text);
    }
}

void line_exec(const std::vector<std::vector<std::string>>& tokens, VarTable& var) {
    int tokens_list = tokens.size();

    for(int line = 0; line < tokens_list; line++) {
        std::vector<std::string> cmd = tokens[line];
        int cmd_size = cmd.size();

        if(cmd[0][0] == '#' && cmd[0][1] == '!') {
            continue;
        }

        else if(cmd[0] == "exit" && cmd_size == 2) {
            int exit_code = stoi(cmd[1]);
            exit(exit_code);
        }

        else if(cmd[0] == "stdout") {
            if(cmd_size == 1) {
                std::cout << "";
            }
            else {
                for(int start_val = 1; start_val < cmd_size; start_val++) {
                    if(cmd[start_val][0] != '$') {
                        parser::std_out(cmd[start_val]);
                    }
                    else {
                        std::string var_name = lexer::get_var_name_from_token(cmd[start_val]);
                        std::string var_type = var.get_type(var_name);
                        if(var_type == "str") {
                            parser::std_out(var.get_from_strings(var_name));
                        }
                        else if(var_type == "num") {
                            std::cout << var.get_from_numbers(var_name);
                        }
                        else if(var_type == "bool") {
                            std::cout << var.get_from_booleans(var_name);
                        }
                    }
                }
            }
        }

        else if(cmd[0] == "stderr" && cmd_size == 2) {
            parser::std_err(cmd[1]);
        }

        else if(cmd[0] == "stdin" && cmd_size == 2) {
            var.read_var(lexer::get_var_name_from_token(cmd[1]));
        }

        else if(cmd[0] == "var") {
            std::vector<std::string> var_data = lexer::lex_variable_declaration(cmd);
            var.var_add(var_data[0], var_data[1], var_data[2], true);
        }

        else if(cmd[0] == "del" && cmd_size == 2) {
            var.var_delete(lexer::get_var_name_from_token(cmd[1]));
        }

        else if(cmd[0][0] == '$') {
            std::vector<std::string> var_data = lexer::lex_variable_reassignment(cmd);
            var.var_add(var.get_type(var_data[0]), var_data[0], var_data[1]);
        }

        else if(cmd[0] == "throw" && cmd_size == 3) {
            errors::throw_err(cmd[1], cmd[2]);
        }

    }
}
