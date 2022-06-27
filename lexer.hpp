#pragma once

#include <vector>

#include "lib/lib_string.hpp"

namespace lexer {
    std::vector<std::string> clean_tokens(std::vector<std::string>& tokens) {
        int tokens_str_size = tokens.size();
        std::vector<std::string> cleaned_tokens;

        for(int each_token = 0; each_token < tokens_str_size; each_token++) {
            if(tokens[each_token] != "") {
                cleaned_tokens.emplace_back(tokens[each_token]);
            }
        }

        return cleaned_tokens;
    }

    std::vector<std::vector<std::string>> tokenize(std::vector<std::string>& source_lines) {
        int lines = source_lines.size();
        std::vector<std::vector<std::string>> all_tokens;

        for(int line = 0; line < lines; line++) {
            std::vector<std::string> tokens = lib::split(source_lines[line], ' ');
            tokens = clean_tokens(tokens);
            all_tokens.emplace_back(tokens);
        }

        return all_tokens;
    }
}
