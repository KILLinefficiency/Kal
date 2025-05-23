#pragma once

#include <vector>
#include "lib_kal_test.hpp"
#include "../config.hpp"

#include "../lexer.hpp"
#include "../exec.hpp"

#define GC delete actual_value; delete found_value;

void make_fn(std::vector<std::string> lines) {
    std::vector<Token> tokens = lexer::tokenize(lines);
}

Value* fn_call(std::vector<std::string> lines) {
    std::vector<Token> tokens = lexer::tokenize(lines);
    return line_exec(tokens, true);
}

void test_fn() {
    Value* actual_value;
    Value* found_value;

    component("Functions");

    title("Return values");
    std::vector<std::string> lines = {
        "fn greet {",
        "<- \"Hello\"",
        "}"
    };
    make_fn(lines);
    actual_value = new String("\"Hello\"");
    found_value = fn_call({ ":greet" });
    check(found_value->print(), actual_value->print());
    GC;

    Functions::gc();

    progress();
}