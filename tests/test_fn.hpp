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

Value* fn_call(std::vector<std::string> lines) {
    std::vector<Token> tokens = lexer::tokenize(lines);
    return line_exec(tokens, true);
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
        "$sum = $sum + $value",
        "if 1 == 1 {",
            "var value = 20",
            "$sum = $sum + $value",
            "if 2 == 2 {",
                "var value = 30",
                "$sum = $sum + $value",
            "}",
        "}"
    };

    tokens = lexer::tokenize(lines);
    line_exec(tokens);

    actual_value = "60";
    found_value = VarTable::print("$sum");
    check(actual_value, found_value);

    actual_value = "10";
    found_value = VarTable::print("$value");
    check(actual_value, found_value);

    VarTable::gc();
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
    found_value = fn_call({ ":greet" });
    CHECK;

    lines = {
        "fn hundred -> {",
        "<- 45 + 55",
        "}"
    };
    make_fn(lines);
    actual_value = new Number("100");
    found_value = fn_call({ ":hundred" });
    CHECK;

    progress();
    title("Parameters");
    lines = {
        "fn greet -> name {",
        "<- \"Hello \" + $name + \"!\"",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":greet \"Kal\"" });
    actual_value = new String("\"Hello Kal!\"");
    CHECK;

    lines = {
        "fn add -> x, y {",
        "<- $x + $y",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":add 200 50" });
    actual_value = new Number("250");
    CHECK;

    lines = {
        "fn times -> n {",
        "<- $n * 10",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":times 5" });
    actual_value = new Number("50");
    CHECK;

    lines = {
        "fn def_args -> a: 10, b: 20 {",
        "<- $a * $b",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":def_args" });
    actual_value = new Number("200");
    CHECK;
    found_value = fn_call({ ":def_args 30" });
    actual_value = new Number("600");
    CHECK;
    found_value = fn_call({ ":def_args 30 30" });
    actual_value = new Number("900");
    CHECK;

    lines = {
        "fn compose {",
        ":times 20 -> x",
        ":add $x 100 -> y",
        "<- $y",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":compose" });
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

    progress();
    title("Recursive Functions");
    lines = {
        "fn fact -> n {",
        "if $n == 0 || $n == 1 {",
        "<- 1",
        "}",
        "<- $n * $(:fact ($n - 1))",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":fact 5" });
    actual_value = new Number("120");
    CHECK;

    lines = {
        "fn fib -> x {",
        "if $x == 0 || $x == 1 {",
        "<- $x",
        "}",
        "<- $(:fib ($x - 1)) + $(:fib ($x - 2))",
        "}"
    };
    make_fn(lines);
    found_value = fn_call({ ":fib 4" });
    actual_value = new Number("3");
    CHECK;
    found_value = fn_call({ ":fib 5" });
    actual_value = new Number("5");
    CHECK;
    found_value = fn_call({ ":fib 6" });
    actual_value = new Number("8");
    CHECK;

    Functions::gc();

    progress();
}