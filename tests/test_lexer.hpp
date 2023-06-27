#pragma once

#include "../lexer.hpp"

void test_lexer() {
    std::vector<Token> found, actual;

    component("Lexer");

    title("lexer::tokenize()");
    std::vector<std::string> source_lines = {
        "var name = \"Kal-El\"",
        "stdout $name \"\\n\"",
    };

    found = lexer::tokenize(source_lines);
    actual = {
        {
            .head = "var",
            .init = { "name", "\"Kal-El\"" }
        },
        {
            .head = "stdout",
            .values = { "$name", "\"\\n\"" }
        }
    };

    check(found[0].head, actual[0].head);
    check(found[1].head, actual[1].head);
    for(uint64_t i = 0; i < actual.size(); i++) {
        for(uint64_t j = 0; j < found[i].init.size(); j++) {
            check(found[i].init[j], actual[i].init[j]);
        }
        for(uint64_t j = 0; j < found[i].values.size(); j++) {
            check(found[i].values[j], actual[i].values[j]);
        }
    }
    progress();
}