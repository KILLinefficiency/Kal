#pragma once

#include "../lexer.hpp"

void test_lexer() {
    component("Lexer");

    title("lexer::tokenize()");
    std::vector<std::string> source_lines = {
        "var name = \"Kal-El\"",
        "stdout $name \"\\n\"",
    };

    std::vector<std::vector<std::string>> found = lexer::tokenize(source_lines);
    std::vector<std::vector<std::string>> actual = { { "var", "name", "\"Kal-El\"" },
                                                     { "stdout", "$name", "\"\\n\"" }
                                                   };

    for(uint64_t outer = 0; outer < found.size(); outer++) {
        for(uint64_t inner = 0; inner < found[outer].size(); inner++) {
            check(found[outer][inner], actual[outer][inner]);
        }
    }
    progress();
}