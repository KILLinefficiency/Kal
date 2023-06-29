#pragma once

#include "../expr_parser.hpp"

void test_expr_parser() {
    std::string expr;
    double found, actual;

    component("Expression Parser");

    title("eval()");
    expr = "1 + 2 * 3";
    actual = 7.0;
    found = eval(expr);
    check(found, actual);

    expr = "(1 + 2) * 3";
    actual = 9.0;
    found = eval(expr);
    check(found, actual);

    expr = "1 == (0 + 1)";
    actual = 1.0;
    found = eval(expr);
    check(found, actual);

    progress();
}