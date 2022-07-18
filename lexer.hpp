#pragma once

#include <vector>

#include "lib/lib_string.hpp"

namespace lexer {
    std::vector<std::vector<std::string>> tokenize(const std::vector<std::string>& source_lines) {
        int lines = source_lines.size();
        std::vector<std::vector<std::string>> all_tokens;

        for(int line = 0; line < lines; line++) {
            std::string current_line = source_lines[line];
            std::vector<std::string> tokens = lib::split(current_line);
            if(tokens[0] == "var" || tokens[0] == "const") {
                tokens = lib::split(current_line, ' ', '"', true);
            }
            all_tokens.emplace_back(tokens);
        }

        return all_tokens;
    }


    std::vector<std::string> lex_variable_declaration(const std::vector<std::string>& tokens, int begin = 1) {
        std::vector<std::string> var_data;

        std::string complete_line = lib::vector_to_string(tokens, "", begin);
        std::vector<std::string> type_split = lib::split(complete_line, ':');
        std::string var_type = type_split[0];

        std::vector<std::string> name_val_split = lib::split(type_split[1], '=');
        std::string var_name = name_val_split[0];
        std::string var_val;

        if(name_val_split.size() == 2) {
            var_val = name_val_split[1];
        }
        else if(var_type == "str") {
            var_val = "";
        }
        else if(var_type == "num") {
            var_val = "0";
        }
        else if(var_type == "bool") {
            var_val = "false";
        }

        var_data = {var_type, var_name, var_val};
        return var_data;
    }

    std::vector<std::string> lex_variable_reassignment(const std::vector<std::string>& tokens) {
        std::vector<std::string> var_reassignment;

        std::string complete_line = lib::vector_to_string(tokens, "");
        std::vector<std::string> var_val_split = lib::split(complete_line, '=');
        std::string var_name = var_val_split[0].substr(1);
        std::string var_val = var_val_split[1];

        var_reassignment = { var_name, var_val };
        return var_reassignment;
    }

    std::string get_var_name_from_token(std::string var_token) {
        std::string var_name = var_token.substr(1);
        return var_name;
    }

    std::vector<std::string> lex_list_values(std::string& str_list, const std::string& var_name) {
        int str_list_size = str_list.size();
        if(str_list[0] != '[' || str_list[str_list_size - 1] != ']') {
            errors::initializer_list_error(var_name);
        }
        std::string values = str_list.substr(1, str_list_size - 2);
        std::vector<std::string> individual_values = lib::split(values, ',');
        return individual_values;
    }

}
