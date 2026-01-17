#pragma once

#include "../exec.hpp"
#include "../lexer.hpp"

#define RUN_CASE(VALUE) actual_string = VALUE; \
    tokens = lexer::tokenize(lines); \
    line_exec(tokens, false, true, false, memory); \
    found_string = VarTable::print("value", memory); \
    check(found_string, actual_string);

void test_control_flow() {
    std::string actual_string, found_string;
    std::vector<std::string> lines;
    std::vector<Token> tokens;

    component("Control Flow");

    title("line_exec() - if");

    lines = {
        "var value = 0",
        "if value == 0 {",
            "value = 1",
        "}"
    };
    RUN_CASE("1");

    lines = {
        "if value == 1 && 1 + 1 == 2 {",
            "value = 2",
        "}"
    };
    RUN_CASE("2");

    lines = {
        "if value == 2 || 2 + 2 == 5 {",
            "value = 3",
        "}"
    };
    RUN_CASE("3");

    lines = {
        "value = 0",
        "if value == 1 {",
            "value = 1",
        "}"
    };
    RUN_CASE("0");

    lines = {
        "value = 0",
        "if value != 1 {",
            "value = 1",
        "}"
    };
    RUN_CASE("1")

    progress();

    title("line_exec() - if-else");

    lines = {
        "value = 0",
        "if value == 1 {",
            "value = 10",
        "}",
        "else {",
            "value = 20",
        "}"
    };
    RUN_CASE("20");

    lines = {
        "value = 1",
        "if value == 1 {",
            "if value + 10 == 11 {",
                "value = 2",
            "}",
        "}",
        "else {",
            "value = 3",
        "}"
    };
    RUN_CASE("2");

    lines = {
        "value = 1",
        "if value == 1 {",
            "if value + 10 == 20 {",
                "value = 2",
            "}",
            "else {",
                "value = 3",
            "}",
        "else {",
            "value = 4",
        "}"
    };
    RUN_CASE("3");

    lines = {
        "value = 0",
        "if value == 1 {",
            "value = 100",
        "}",
        "else {",
            "if value == 0 {",
                "value = 10",
            "}",
            "else {",
                "value = 20",
            "}",
        "}"
    };
    RUN_CASE("10");

    lines = {
        "value = 0",
        "if value == 1 {",
            "value = 100",
        "}",
        "else {",
            "if value == 11 {",
                "value = 10",
            "}",
            "else {",
                "value = 20",
            "}",
        "}"
    };
    RUN_CASE("20");

    lines = {
        "value = 0",
        "if 0 {",
            "value = 1",
        "}",
        "var other_value = 100",
        "else {",
            "value = 2",
        "}"
    };
    RUN_CASE("2");

    lines = {
        "value = 0",
        "if 1 {",
            "if value == 1 {",
                "value = 1",
            "}",
            "other_value = 200",
            "else {",
                "value = 2",
            "}",
        "}"
    };
    RUN_CASE("2");

    lines = {
        "value = 0",
        "if value == 0 {",
            "value = 1",
        "}",
        "if 0 {",
            "value = 3",
        "}",
        "else {",
            "value = 2",
        "}",
        "else {",
            "value = 4",
        "}"
    };
    RUN_CASE("2");

    lines = {
        "value = 0",
        "if value == 1 {",
            "value = 1",
        "}",
        "if 1 {",
            "value = 3",
        "}",
        "else {",
            "value = 2",
        "}",
        "else {",
            "value = 4",
        "}"
    };
    RUN_CASE("4");

    lines = {
        "value = 0",
        "if value == 0 {",
            "value = 1",
        "}",
        "if 0 {",
            "value = 2",
        "}",
        "else {",
            "value = 3",
        "}",
        "else {",
            "value = 4",
        "}"
    };
    RUN_CASE("3");

    progress();
    title("line_exec() - if-elif-else");

    lines = {
        "value = 0",
        "var data = 3",
        "if data == 1 {",
            "value = 100",
        "}",
        "elif data == 2 {",
            "value = 200",
        "}",
        "elif data == 3 {",
            "value = 300",
        "}",
        "elif data == 4 {",
            "value = 400",
        "}",
        "else {",
            "value = 999",
        "}"
    };
    RUN_CASE("300");

    VarTable::gc(0, memory);
    progress();

    title("line_exec() - loop");

    lines = {
        "var i = 1, value = 0",
        "loop i <= 10 {",
            "value = value + i",
            "i = i + 1",
        "}"
    };
    RUN_CASE("55");

    lines = {
        "i = 1",
        "value = 0",
        "loop i <= 10 {",
            "if i % 2 == 0 {",
                "value = value + i",
            "}",
            "i = i + 1",
        "}"
    };
    RUN_CASE("30");

    lines = {
        "i = 1",
        "value = 0",
        "loop i <= 10 {",
            "if i == 4 || i == 9 {",
                "i = i + 1",
                "continue",
            "}",
            "value = value + i",
            "i = i + 1",
        "}"
    };
    RUN_CASE("42");

    lines = {
        "i = 1",
        "value = 0",
        "loop i <= 10 {",
            "if i == 6 {",
                "break",
            "}",
            "value = value + i",
            "i = i + 1",
        "}"
    };
    RUN_CASE("15");

    lines = {
        "i = 1",
        "value = 0",
        "loop i <= 5 {",
            "var j = 1",
            "loop j <= 5 {",
                "if i == j {",
                    "j = j + 1",
                    "continue",
                "}",
                "value = value + i + j",
                "j = j + 1",
            "}",
            "i = i + 1",
        "}"
    };
    RUN_CASE("120");

    lines = {
        "value = 0",
        "loop p = 1 -- p <= 5 -- p = p + 1 {",
            "value = value + p",
        "}"
    };
    RUN_CASE("15");

    lines = {
        "value = 0",
        "loop a = 1 -- a <= 5 -- a = a + 1 {",
            "loop b = 1 -- b <= 5 -- b = b + 1 {",
                "if a == b {",
                    "continue",
                "}",
                "value = value + a + b",
            "}",
        "}"
    };
    RUN_CASE("120");

    lines = {
        "value = 0",
        "var itr = 1",
        "loop -- itr <= 5 -- itr = itr + 1 {",
            "value = value + itr",
        "}"
    };
    RUN_CASE("15");

    lines = {
        "value = 0",
        "loop idx = 1 -- -- idx = idx + 1 {",
            "if idx == 6 {",
                "break",
            "}",
            "value = value + idx",
        "}"
    };
    RUN_CASE("15");

    lines = {
        "value = 0",
        "loop index = 1 -- index <= 5 {",
            "value = value + index",
            "index = index + 1",
        "}",
    };
    RUN_CASE("15");

    lines = {
        "value = 0",
        "loop a = 1 -- -- {",
            "if a == 6 {",
                "break",
            "}",
            "value = value + a",
            "a = a + 1",
        "}"
    };
    RUN_CASE("15");

    lines = {
        "value = 0",
        "var b = 1",
        "loop -- b <= 5 -- {",
            "value = value + b",
            "b = b + 1",
        "}"
    };
    RUN_CASE("15");

    lines = {
        "value = 0",
        "var c = 1",
        "loop -- -- c = c + 1 {",
            "if c == 6 {",
                "break",
            "}",
            "value = value + c",
        "}"
    };
    RUN_CASE("15");

    VarTable::gc(0, memory);
    lines = {
        "var value = 0",
        "var info = [1, 2, 3, 4, 5]",
        "loop x in info {",
            "value = value + x",
        "}"
    };
    RUN_CASE("15");

    lines = {
        "value = 0",
        "loop x in [1, 2, 3, 4, 5] {",
            "value = value + x",
        "}"
    };
    RUN_CASE("15");

    lines = {
        "value = 0",
        "info = [1, 2, 3, 4, 5]",
        "loop &x in info {",
            "x = x * 10",
        "}",
        "loop y in info {",
            "value = value + y",
        "}"
    };
    RUN_CASE("150");

    lines = {
        "value = 0",
        "info = [[0, 1, 2], [3, 4, 5]]",
        "loop &x in info {",
            "loop &y in x {",
                "y = y + 1",
            "}",
        "}",

        "loop a in info {",
            "loop b in a {",
                "value = value + b",
            "}",
        "}"
    };
    RUN_CASE("21");

    lines = {
        "value = 0",
        "info = [1, 2, 3]",
        "loop x in info {",
            "loop i = 1 -- i < 4 -- i = i + 1 {",
                "value = value + (x + i)",
            "}",
        "}"
    };
    RUN_CASE("36");

    lines = {
        "value = 0",
        "info = [1, 2, 3]",
        "loop i = 1 -- i < 4 -- i = i + 1 {",
            "loop x in info {",
                "value = value + (i + x)",
            "}",
        "}"
    };
    RUN_CASE("36");

    lines = {
        "value = 0",
        "var shadowed = 10",
        "loop shadowed = 1 -- shadowed <= 5 -- shadowed = shadowed + 1 {",
            "value = value + shadowed",
        "}"
    };
    RUN_CASE("15");
    std::string found_value = VarTable::print("shadowed", memory);
    std::string actual_value = "10";
    check(found_value, actual_value);

    VarTable::gc(0, memory);
    lines = {
        "var value = 0",
        "if (test_value := 100) > 50 {",
            "value = test_value",
        "}"
    };
    RUN_CASE("100");

    lines = {
        "value = 0",
        "var test = 0",
        "if 1 == 1 {",
            "var x = (test := 10) * 20",
            "value = test",
        "}"
    };
    RUN_CASE("10");

    VarTable::gc(0, memory);
    progress();
}