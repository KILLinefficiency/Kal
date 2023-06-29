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
}