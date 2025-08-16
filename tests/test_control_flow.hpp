#pragma once

#include "../exec.hpp"
#include "../lexer.hpp"

#define RUN_CASE(VALUE) actual_string = VALUE; \
    tokens = lexer::tokenize(lines); \
    line_exec(tokens); \
    found_string = VarTable::print("$value"); \
    check(found_string, actual_string);

void test_control_flow() {
    std::string actual_string, found_string;
    std::vector<std::string> lines;
    std::vector<Token> tokens;

    component("Control Flow");

    title("line_exec() - if");

    lines = {
        "var value = 0",
        "if $value == 0 {",
            "$value = 1",
        "}"
    };
    RUN_CASE("1");

    lines = {
        "if $value == 1 && 1 + 1 == 2 {",
            "$value = 2",
        "}"
    };
    RUN_CASE("2");

    lines = {
        "if $value == 2 || 2 + 2 == 5 {",
            "$value = 3",
        "}"
    };
    RUN_CASE("3");

    lines = {
        "$value = 0",
        "if $value == 1 {",
            "$value = 1",
        "}"
    };
    RUN_CASE("0");

    lines = {
        "$value = 0",
        "if $value != 1 {",
            "$value = 1",
        "}"
    };
    RUN_CASE("1")

    progress();

    title("line_exec() - if-else");

    lines = {
        "$value = 0",
        "if $value == 1 {",
            "$value = 10",
        "}",
        "else {",
            "$value = 20",
        "}"
    };
    RUN_CASE("20");

    lines = {
        "$value = 1",
        "if $value == 1 {",
            "if $value + 10 == 11 {",
                "$value = 2",
            "}",
        "}",
        "else {",
            "$value = 3",
        "}"
    };
    RUN_CASE("2");

    lines = {
        "$value = 1",
        "if $value == 1 {",
            "if $value + 10 == 20 {",
                "$value = 2",
            "}",
            "else {",
                "$value = 3",
            "}",
        "else {",
            "$value = 4",
        "}"
    };
    RUN_CASE("3");

    lines = {
        "$value = 0",
        "if $value == 1 {",
            "$value = 100",
        "}",
        "else {",
            "if $value == 0 {",
                "$value = 10",
            "}",
            "else {",
                "$value = 20",
            "}",
        "}"
    };
    RUN_CASE("10");

    lines = {
        "$value = 0",
        "if $value == 1 {",
            "$value = 100",
        "}",
        "else {",
            "if $value == 11 {",
                "$value = 10",
            "}",
            "else {",
                "$value = 20",
            "}",
        "}"
    };
    RUN_CASE("20");

    lines = {
        "$value = 0",
        "if 0 {",
            "$value = 1",
        "}",
        "var other_value = 100",
        "else {",
            "$value = 2",
        "}"
    };
    RUN_CASE("2");

    lines = {
        "$value = 0",
        "if 1 {",
            "if $value == 1 {",
                "$value = 1",
            "}",
            "$other_value = 200",
            "else {",
                "$value = 2",
            "}",
        "}"
    };
    RUN_CASE("2");

    lines = {
        "$value = 0",
        "if $value == 0 {",
            "$value = 1",
        "}",
        "if 0 {",
            "$value = 3",
        "}",
        "else {",
            "$value = 2",
        "}",
        "else {",
            "$value = 4",
        "}"
    };
    RUN_CASE("2");

    lines = {
        "$value = 0",
        "if $value == 1 {",
            "$value = 1",
        "}",
        "if 1 {",
            "$value = 3",
        "}",
        "else {",
            "$value = 2",
        "}",
        "else {",
            "$value = 4",
        "}"
    };
    RUN_CASE("4");

    lines = {
        "$value = 0",
        "if $value == 0 {",
            "$value = 1",
        "}",
        "if 0 {",
            "$value = 2",
        "}",
        "else {",
            "$value = 3",
        "}",
        "else {",
            "$value = 4",
        "}"
    };
    RUN_CASE("3");

    VarTable::gc();
    progress();

    title("line_exec() - loop");

    lines = {
        "var i = 1, value = 0",
        "loop $i <= 10 {",
            "$value = $value + $i",
            "$i = $i + 1",
        "}"
    };
    RUN_CASE("55");

    lines = {
        "$i = 1",
        "$value = 0",
        "loop $i <= 10 {",
            "if $i % 2 == 0 {",
                "$value = $value + $i",
            "}",
            "$i = $i + 1",
        "}"
    };
    RUN_CASE("30");

    lines = {
        "$i = 1",
        "$value = 0",
        "loop $i <= 10 {",
            "if $i == 4 || $i == 9 {",
                "$i = $i + 1",
                "continue",
            "}",
            "$value = $value + $i",
            "$i = $i + 1",
        "}"
    };
    RUN_CASE("42");

    lines = {
        "$i = 1",
        "$value = 0",
        "loop $i <= 10 {",
            "if $i == 6 {",
                "break",
            "}",
            "$value = $value + $i",
            "$i = $i + 1",
        "}"
    };
    RUN_CASE("15");

    lines = {
        "$i = 1",
        "$value = 0",
        "loop $i <= 5 {",
            "var j = 1",
            "loop $j <= 5 {",
                "if $i == $j {",
                    "$j = $j + 1",
                    "continue",
                "}",
                "$value = $value + $i + $j",
                "$j = $j + 1",
            "}",
            "$i = $i + 1",
        "}"
    };
    RUN_CASE("120");

    VarTable::gc();
    progress();
}