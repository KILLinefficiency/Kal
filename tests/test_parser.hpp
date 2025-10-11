#pragma once

#include "../parser.hpp"

void test_parser() {
    int index;
    uint64_t i;
    std::string text, actual, found;
    std::vector<std::string> actual_vec, found_vec;

    component("Parser");

    title("parser::parse_number()");
    index = 3;
    text = "abc3.14xyz";
    actual = "3.14";
    found = parser::parse_number(text, index);
    check(found, actual);
    progress();

    title("parser::parse_init()");
    index = 0;
    text = "name, age";
    actual_vec = { "name", "null", "age", "null" };
    found_vec = parser::parse_init(text, index);
    for(i = 0; i < found_vec.size(); i++) {
        check(found_vec[i], actual_vec[i]);
    }

    index = 0;
    text = "name = \"Superman\", age = 25,\naddress, friends = [\"Batman\", \"The Flash\"],\tpowers = null";
    actual_vec = { "name", "\"Superman\"", "age", "25", "address", "null", "friends", "[\"Batman\", \"The Flash\"]", "powers", "null" };
    found_vec = parser::parse_init(text, index);
    for(i = 0; i < found_vec.size(); i++) {
        check(found_vec[i], actual_vec[i]);
    }
    progress();

    title("parser::parse_string()");
    index = 0;
    text = "\"Hello\" \"World\"";
    actual = "\"Hello\"";
    found = parser::parse_string(text, index);
    check(found, actual);

    index = 8;
    actual = "\"World\"";
    found = parser::parse_string(text, index);
    check(found, actual);
    progress();

    title("parser::parse_variable()");
    index = 0;
    text = "data";
    actual = "data";
    found = parser::parse_variable(text, index);
    check(found, actual);
    index = 0;
    text = "data = 10";
    actual = "data";
    found = parser::parse_variable(text, index);
    check(found, actual);
    index = 10;
    text = "var ref = &data";
    actual = "&data";
    found = parser::parse_variable(text, index);
    check(found, actual);
    index = 15;
    text = "var new_data = data[10]";
    actual = "data[10]";
    found = parser::parse_variable(text, index);
    check(found, actual);
    index = 15;
    text = "static value = outer[inner[25]] + 75";
    actual = "outer[inner[25]]";
    found = parser::parse_variable(text, index);
    check(found, actual);
    index = 0;
    text = "outer[inner[25]]";
    actual = "outer";
    found = parser::parse_variable(text, index, false);
    check(found, actual);
    progress();
}