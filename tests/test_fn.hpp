#pragma once

#include <vector>
#include "lib_kal_test.hpp"
#include "../config.hpp"

#include "../lexer.hpp"
#include "../exec.hpp"

#define CHECK check(found_value->print(), actual_value->print()); delete actual_value; delete found_value;

void make_fn(std::vector<std::string> lines) {
    lexer::tokenize(lines);
}

Value* fn_call(std::vector<std::string> lines, Memory& memory) {
    std::vector<Token> tokens = lexer::tokenize(lines);
    return line_exec(tokens, true, true, false, memory);
}

void test_shadowing() {
    component("Variable Shadowing");

    std::vector<std::string> lines;
    std::string actual_value;
    std::string found_value;
    std::vector<Token> tokens;

    title("Variable Shadowing");

    lines = {
        "var sum = 0",
        "var value = 10",
        "sum = sum + value",
        "if 1 == 1 {",
            "var value = 20",
            "sum = sum + value",
            "if 2 == 2 {",
                "var value = 30",
                "sum = sum + value",
            "}",
        "}"
    };

    tokens = lexer::tokenize(lines);
    line_exec(tokens, false, true, false, memory);

    actual_value = "60";
    found_value = VarTable::print("sum", memory);
    check(actual_value, found_value);

    actual_value = "10";
    found_value = VarTable::print("value", memory);
    check(actual_value, found_value);

    VarTable::gc(0, memory);
    progress();
}

void test_fn() {
    Value* actual_value;
    Value* found_value;
    std::vector<std::string> lines;

    component("Functions");

    title("Return values");
    lines = {
        "fn greet {",
        "<- \"Hello\"",
        "}"
    };
    make_fn(lines);
    actual_value = new String("\"Hello\"");
    found_value = fn_call({ ":greet" }, memory);
    CHECK;

    lines = {
        "fn hundred -> {",
        "<- 45 + 55",
        "}"
    };
    make_fn(lines);
    actual_value = new Number("100");
    found_value = fn_call({ ":hundred" }, memory);
    CHECK;

    progress();
    title("Parameters");
    lines = {
        "fn greet -> name {",
        "<- \"Hello \" + name + \"!\"",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":greet \"Kal\"" }, memory);
    actual_value = new String("\"Hello Kal!\"");
    CHECK;

    lines = {
        "fn add -> x, y {",
        "<- x + y",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":add 200 50" }, memory);
    actual_value = new Number("250");
    CHECK;

    lines = {
        "fn times -> n {",
        "<- n * 10",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":times 5" }, memory);
    actual_value = new Number("50");
    CHECK;

    lines = {
        "fn def_args -> a: 10, b: 20 {",
        "<- a * b",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":def_args" },memory);
    actual_value = new Number("200");
    CHECK;
    found_value = fn_call({ ":def_args 30" }, memory);
    actual_value = new Number("600");
    CHECK;
    found_value = fn_call({ ":def_args 30 30" }, memory);
    actual_value = new Number("900");
    CHECK;

    lines = {
        "fn compose {",
        ":times 20 -> x",
        ":add x 100 -> y",
        "<- y",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":compose" }, memory);
    actual_value = new Number("300");
    CHECK;

    /*lines = {
        "fn add_again {",
        "<- $(:add $(:add 45 55) $(:add 35 15))",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":add_again" });
    actual_value = new Number("150");
    CHECK;*/

    // TODO: Add tests for pass by reference.

    progress();
    title("Recursive Functions");
    lines = {
        "fn fact -> n {",
        "if n == 0 || n == 1 {",
        "<- 1",
        "}",
        "<- n * $(:fact (n - 1))",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":fact 5" }, memory);
    actual_value = new Number("120");
    CHECK;

    lines = {
        "fn fib -> x {",
        "if x == 0 || x == 1 {",
        "<- x",
        "}",
        "<- $(:fib (x - 1)) + $(:fib (x - 2))",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":fib 4" }, memory);
    actual_value = new Number("3");
    CHECK;
    found_value = fn_call({ ":fib 5" }, memory);
    actual_value = new Number("5");
    CHECK;
    found_value = fn_call({ ":fib 6" }, memory);
    actual_value = new Number("8");
    CHECK;

    progress();
    title("defer");

    lines = {
        "fn mul -> n {",
            "n = n * 10",
        "}",

        "fn incr -> n {",
            "n = n + 10",
        "}",

        "fn test_defer_a {",
            "var n = 5",
            "defer $(:mul &n)",
            "defer $(:incr &n)",
            "<- n",
        "}",

        "fn test_defer_b {",
            "var n = 5",
            "defer $(:mul &n) $(:incr &n)",
            "<- n",
        "}"
    };
    make_fn(lines);

    found_value = fn_call({ ":test_defer_a" }, memory);
    actual_value = new Number("150");
    CHECK;

    found_value = fn_call({ ":test_defer_b" }, memory);
    actual_value = new Number("60");
    CHECK;

    VarTable::set("test_value", "10", nullptr, VAR, false, 0, false, memory);
    lines = {
        "fn test_defer_c {",
            "defer $(:incr &test_value)",
        "}"
    };
    make_fn(lines);
    fn_call({ ":test_defer_c" }, memory);
    found_value = VarTable::get("test_value", {}, true, true, true, memory);
    actual_value = new Number("20");
    CHECK;

    lines = {
        "fn test_defer_d {",
            "defer $(:mul &n)",
            "var n = 5",
            "<- n",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":test_defer_d" }, memory);
    actual_value = new Number("50");
    CHECK;

    Functions::gc();

    progress();
    title("Spread Operator");

    lines = {
        "fn add -> ...data {",
            "var sum = 0",
            "loop x in data {",
                "sum = sum + x",
            "}",
            "<- sum",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":add 1 2 3" }, memory);
    actual_value = new Number("6");
    CHECK;

    found_value = fn_call({ ":add 1 2 3 4" }, memory);
    actual_value = new Number("10");
    CHECK;

    found_value = fn_call({ ":add 1 2 3 4 5" }, memory);
    actual_value = new Number("15");
    CHECK;

    lines = {
        "fn test_spread {",
            "var data = [1, 2, 3]",
            ":add ...data -> sum",
            "<- sum",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":test_spread" }, memory);
    actual_value = new Number("6");
    CHECK;

    lines = {
        "fn test_spread_1 {",
            ":add ...[1, 2, 3] -> sum",
            "<- sum",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":test_spread_1" }, memory);
    actual_value = new Number("6");
    CHECK;

    lines = {
        "fn add_two -> x, y {",
            "<- x + y",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":add_two ...#(x -> 45, y -> 55)" }, memory);
    actual_value = new Number("100");
    CHECK;

    lines = {
        "fn test_spread_2 {",
            "var data = #(x -> 45, y -> 55)",
            ":add_two ...data -> sum",
            "<- sum",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":test_spread_2" }, memory);
    actual_value = new Number("100");
    CHECK;

    Functions::gc();
    progress();
}