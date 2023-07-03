#pragma once

#include "../expr_parser.hpp"

void test_expr_parser() {
    std::string found;
    std::vector<std::string> expr, actual;

    component("Expression Parser");

    title("eval()");
    expr = {
        "\"hello\" == \"hello\"",
        "\"hello\" != \"hello\"",
        "\"hello\" == \"hey\"",
        "\"hello\" != \"hey\"",
        "\"a\" + \"b\"",
        "\"a\" * 3",
        "\"a\" + \"b\" * 3",
        "\"a\" + \"b\" * 3 == \"abbb\"",
        "(\"a\" + \"b\") * 3",
        "(\"a\" + \"b\") * 3 == \"ababab\"",
        "(\"a\" + \"b\") * (1 + 2) == \"ababab\"",
        "\"hell\" + \"o\" == \"hel\" + \"lo\"",
        "\"a\" * 1 + \"b\" * 2 + \"c\" * 3",
        "\"a\" * 1 + \"b\" * 2 + \"c\" * 3 == \"abbccc\""
    };
    
    actual = { "1", "0", "0", "1", "\"ab\"", "\"aaa\"", "\"abbb\"", "1", "\"ababab\"", "1", "1", "1", "\"abbccc\"", "1" };

    for(uint64_t i = 0; i < expr.size(); i++) {
        found = eval(expr[i]);
        check(found, actual[i]);
    }

    progress();
}