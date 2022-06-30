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
}
