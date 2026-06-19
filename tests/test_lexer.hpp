#pragma once

#include "../lexer.hpp"

void test_lexer() {
    std::vector<Token> found, actual;

    component("Lexer");

    title("lexer::tokenize()");
    std::vector<std::string> source_lines = {
        "var name = \"Kal-El\"",
        "stdout name \"\\n\"",
    };

    std::string line = "var name = \"Kal-El\"";
    found = lexer::tokenize(source_lines);
    actual = {
        {
            .head = "var",
            .init = { "name", "\"Kal-El\"" },
            .line = &line
        },
        {
            .head = "stdout",
            .values = { "name", "\"\\n\"" },
            .line = &line
        }
    };

    for(uint64_t i = 0; i < found.size(); i++) {
        check_token(found[i], actual[i]);
    }

    progress();
}