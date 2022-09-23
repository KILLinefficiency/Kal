#pragma once

#include "lexer.hpp"
#include "errors.hpp"
#include "variable.hpp"
#include "lib/lib_style.hpp"
#include "lib/lib_string.hpp"

namespace parser {
    void std_out(std::string out_text) {
        std::cout << lib::render_escape_chars(out_text);
    }

    void std_err(std::string err_text) {
        std::cerr << lib::render_escape_chars(err_text);
    }
}

void line_exec(std::vector<std::vector<std::string>>& tokens, VarTable& var, const std::vector<std::string>& prog_args) {
    bool warn = true;
    int tokens_list = tokens.size();

    int args_len = prog_args.size();
    var.var_add("var", "num", "[args#len]", std::to_string(args_len));
    for(int arg_count = 0; arg_count < args_len; arg_count++) {
        std::string args_identifier = "[args#" + std::to_string(arg_count) + "]";
        var.var_add("var", "str", args_identifier, prog_args[arg_count]);
    }
    var.add_structure("args", "list");

    for(int line = 0; line < tokens_list; line++) {
        std::vector<std::string>& cmd = tokens[line];
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

        else if(cmd[0] == "warn") {
            if(cmd_size == 1) {
                warn = !warn;
            }
            else if(cmd_size == 2) {
                if(cmd[1] == "off") {
                    warn = false;
                }
                else if(cmd[1] == "on") {
                    warn = true;
                }
            }
        }

        else if(cmd[0] == "style") {
            for(int style_itr = 1; style_itr < cmd_size; style_itr++) {
                std::string passed_style = cmd[style_itr];
                if(passed_style[0] == '$') {
                    passed_style = var.eval_var(passed_style);
                }
                std::string current_style = style::style[passed_style];

                if(current_style != "") {
                    std::cout << current_style;
                }
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
                        /*
                        std::string var_name = lexer::get_var_name_from_token(cmd[start_val]);
                        std::string var_type = var.get_type(var_name);
                        if(var_type == "str") {
                            parser::std_out(var.get_from_strings(var_name));
                        }
                        else if(var_type == "num") {
                            std::cout << var.get_from_numbers(var_name);
                        }
                        */
                        if(var.get_structure_type(cmd[start_val].substr(1)) == "list") {
                            std::cout << var.print_list(cmd[start_val].substr(1));
                            continue;
                        }
                        std::string str_value = var.eval_var(cmd[start_val]);
                        parser::std_out(str_value);
                        //std::cout << var.get_current_str() << std::endl;
                        /*std::string var_name = lexer::get_var_name_from_token(cmd[start_val]);
                        std::cout << cmd[start_val] << std::endl;
                        std::cout << var_name << std::endl;
                        std::string var_type = var.get_type(var_name);
                        std::cout << var_type << std::endl;
                        if(var_type == "num") {
                            parser::std_out(std::to_string(var.get_current_num()));
                        }
                        else if(var_type == "str") {
                            parser::std_out(var.get_current_str());
                        }*/
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

            if(cmd[0] == "const" && var_data[2] == "" && warn) {
                warnings::const_uninitialized_warning(var_data[1]);
            }

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
            }

            var.var_add(var.get_mem_type(var_data[0]), var.get_type(var_data[0]), var_data[0], second_var_val);
        }

        else if(cmd[0] == "list") {
            std::vector<std::string> list_data = lexer::lex_list_declaration(cmd);
            std::string& list_name = list_data[0];
            int list_len = list_data.size();
            var.var_add("var", "num", "[" + list_name + "#len]", std::to_string(list_len));
            for(int each_item = 1; each_item < list_len; each_item++) {
                std::string identifier = "[" + list_name + "#" + std::to_string(each_item - 1) + "]";
                var.var_add("var", "str", identifier, list_data[each_item]);
            }
            var.add_structure(list_name, "list");
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
        
        else if(cmd[0] == "read" || cmd[0] == "write") {
            std::string file_code = lib::vector_to_string(cmd, "", 1);
            std::vector<std::string> tok = lib::str_split(file_code , "->");
            if(cmd[0] == "read") {
                std::string& file_path = tok[0];
                std::string& hold_var = tok[1];
                if(file_path[0] == '$') {
                    file_path = var.get_from_strings(lexer::get_var_name_from_token(file_path));
                }
                hold_var = lexer::get_var_name_from_token(hold_var);
                std::string read_text = lib::render_escape_chars(lib::read_file(file_path));

                var.var_add("var", "str", hold_var, read_text);
            }
            else if(cmd[0] == "write") {
                std::string& write_string = tok[0];
                std::string& dest_file = tok[1];
                if(write_string[0] == '$') {
                    write_string = var.get_from_strings(lexer::get_var_name_from_token(write_string));
                }
                if(dest_file[0] == '$') {
                    dest_file = var.get_from_strings(lexer::get_var_name_from_token(dest_file));
                }
                write_string = lib::render_escape_chars(write_string);
                lib::write_file(dest_file, write_string);
            }
        }

        else {
            errors::unidentified_keyword(cmd[0]);
        }

    }

    std::cout << style::style["reset"];
}
