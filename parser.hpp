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

        else if(cmd[0] == "exit") {
            if(cmd_size == 1) {
                exit(0);
            }
            else {
                int exit_code = stoi(cmd[1]);
                exit(exit_code);
            }
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
            std::string var_to_read = lexer::get_var_name_from_token(cmd[1]);
            if(var.get_mem_type(var_to_read) == "const") {
                errors::change_const_var_error(var_to_read);
            }
            var.read_var(lexer::get_var_name_from_token(cmd[1]));
        }

        else if(cmd[0] == "var" || cmd[0] == "const") {
            std::vector<std::string> var_data = lexer::lex_variable_declaration(cmd);

            if(var_data[2] == "") {
                std::vector<std::string> multiple_vars = lib::split(var_data[1], ',');
                int total_vars = multiple_vars.size();
                for(int add_var = 0; add_var < total_vars; add_var++) {
                    var.var_add(cmd[0], var_data[0], multiple_vars[add_var], var_data[2], true);
                }
                continue;
            }

            if(var_data[2][0] == '$') {
                std::string second_var = lexer::get_var_name_from_token(var_data[2]);
                std::string second_var_type = var.get_type(second_var);

                if(var_data[0] != second_var_type) {
                    errors::types_incompatible_error(var_data[1], var_data[0], second_var, second_var_type);
                }
                if(var_data[0] == "str") {
                    var_data[2] = var.get_from_strings(second_var);
                }
                else if(var_data[0] == "num") {
                    double num_val = var.get_from_numbers(second_var);
                    var_data[2] = std::to_string(num_val);
                }
                else if(var_data[0] == "bool") {
                    bool bool_val = var.get_from_booleans(second_var);
                    if(bool_val) {
                        var_data[2] = "true";
                    }
                    else {
                        var_data[2] = "false";
                    }
                }
            }

            var.var_add(cmd[0], var_data[0], var_data[1], var_data[2], true);
        }

        else if(cmd[0][0] == '$') {
            std::vector<std::string> var_data = lexer::lex_variable_reassignment(cmd);

            std::string first_var_type = var.get_type(var_data[0]);
            std::string second_var_val = var_data[1];
            if(second_var_val[0] == '$') {
                std::string second_var = lexer::get_var_name_from_token(var_data[1]);
                std::string second_var_type = var.get_type(second_var);
                if(first_var_type != second_var_type) {
                    errors::types_incompatible_error(var_data[0], first_var_type, second_var, second_var_type);
                }

                if(first_var_type == "str") {
                    second_var_val = var.get_from_strings(second_var);
                }
                else if(first_var_type == "num") {
                    double num_val = var.get_from_numbers(second_var);
                    second_var_val = std::to_string(num_val);
                }
                else if(first_var_type == "bool") {
                    bool bool_val = var.get_from_booleans(second_var);
                    if(bool_val) {
                        second_var_val = "true";
                    }
                    else {
                        second_var_val = "false";
                    }
                }
            }

            var.var_add(var.get_mem_type(var_data[0]), var.get_type(var_data[0]), var_data[0], second_var_val);
        }

        else if(cmd[0] == "del" && cmd_size == 2) {
            var.var_delete(lexer::get_var_name_from_token(cmd[1]));
        }

        else if(cmd[0] == "throw" && cmd_size == 3) {
            errors::throw_err(cmd[1], cmd[2]);
        }

        else if(cmd[0] == "concat") {
            std::string concat_str = "";
            std::string concat_code = lib::vector_to_string(cmd, " ", 1, "\"");
            std::vector<std::string> tok = lib::str_split(concat_code, "->");

            int tok_size = tok.size();
            std::string& passed_strings = tok[0];
            std::vector<std::string> strings = lib::split(passed_strings, ' ');

            int str_size = strings.size();
            bool literal_first = (strings[0][0] != '$');
            for(int str_itr = 0; str_itr < str_size; str_itr++) {
                std::string current_value = strings[str_itr];
                if(current_value[0] == '$') {
                    std::string var_name = lexer::get_var_name_from_token(current_value);
                    if(var.get_type(var_name) != "str") {
                        errors::expected_type_error("str");
                    }
                    current_value = var.get_from_strings(var_name);
                }

                concat_str += lib::render_escape_chars(current_value);
            }

            if(tok_size == 1) {
                if(literal_first) {
                    errors::cannot_write_to_literal_error(strings[0]);
                }
                std::string first_var_name = lexer::get_var_name_from_token(strings[0]);
                var.var_add("var", "str", first_var_name, concat_str);
            }

            else if(tok_size == 2) {
                int var_start = 0;
                while(tok[1][var_start] != '$') {
                    var_start++;
                }
                std::string destination_string = lexer::get_var_name_from_token(tok[1].substr(var_start, tok[1].size() - (var_start + 2)));
                var.var_add("var", "str", destination_string, concat_str);
            }
        }

        else {
            errors::unidentified_keyword(cmd[0]);
        }

    }
}
