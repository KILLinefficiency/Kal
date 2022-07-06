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
        std::string var_val = name_val_split[1];

        var_data = {var_type, var_name, var_val};
        return var_data;
    }
}
