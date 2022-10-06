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

        var_data = {var_type, var_name, var_val};
        return var_data;
    }

    std::vector<std::string> lex_variable_reassignment(const std::vector<std::string>& tokens) {
        std::vector<std::string> var_reassignment;

        std::string complete_line = lib::vector_to_string(tokens, "");
        std::vector<std::string> var_val_split = lib::split(complete_line, '=');
        std::string var_name = var_val_split[0];
        std::string var_val = var_val_split[1];

        var_reassignment = { var_name, var_val };
        return var_reassignment;
    }

    std::string get_var_name_from_token(std::string var_token) {
        std::string var_name = var_token.substr(1);
        return var_name;
    }

    std::vector<std::string> lex_list_declaration(std::vector<std::string>& tokens) {
        std::vector<std::string> list_name_elements = {};
        std::string list_code = lib::vector_to_string(tokens, " ", 1);
        std::vector<std::string> list_data = lib::split(list_code, ':');
        std::string& list_type = list_data[0];
        std::vector<std::string> rest_list = lib::split(list_data[1], '=');
        std::string list_name = lib::trim_leading(lib::trim_trailing(rest_list[0]));
        rest_list[1] = lib::trim_leading(lib::trim_trailing(rest_list[1]));
        int list_size = rest_list[1].size();
        if(rest_list[1] != "[]" && rest_list[1][0] != '$') {
            rest_list[1] = rest_list[1].substr(1, list_size - 2);
        }
        list_name_elements.emplace_back(list_type);
        list_name_elements.emplace_back(list_name);
        if(rest_list[1][0] == '$') {
            list_name_elements.emplace_back(rest_list[1]);
            return list_name_elements;
        }

        if(rest_list[1] != "[]") {
            std::vector<std::string> elements = lib::split(rest_list[1], ',');
            int list_len = elements.size();
            for(int each_element = 0; each_element < list_len; each_element++) {
                elements[each_element] = lib::trim_leading(lib::trim_trailing(elements[each_element]));
                list_name_elements.emplace_back(elements[each_element]);
            }
        }

        return list_name_elements;
    }

    std::vector<std::string> lex_extend_list(std::vector<std::string>& tokens) {
        std::vector<std::string> extend_list_tokens;
        std::string extend_code = lib::vector_to_string(tokens, " ", 1);
        std::cout << extend_code << std::endl;

        return extend_list_tokens;
    }

}
