#pragma once

#include <vector>

#include "lib/lib_string.hpp"

namespace lexer {
    std::vector<std::vector<std::string>> tokenize(std::vector<std::string>& source_lines) {
        int lines = source_lines.size();
        std::vector<std::vector<std::string>> all_tokens;

        for(int line = 0; line < lines; line++) {
            std::cout << source_lines[line] << std::endl;
            std::vector<std::string> tokens = lib::split(source_lines[line], ' ');
            for(auto x : tokens) {
                std::cout << x << std::endl;
            }
            all_tokens.emplace_back(tokens);
        }

        return all_tokens;
    }
}
