#pragma once

#include "../preprocessor.hpp"

void test_preprocessor() {
    int index;
    std::string text, actual, found;
    std::vector<std::string> orignal, other, found_vec, actual_vec;

    component("Preprocessor");

    title("preproc::squash_vector()");
    orignal = { "a", "b", "c" };
    other = { "x", "y", "z" };
    index = 1;
    actual_vec = { "a", "x", "y", "z", "c" };
    preproc::squash_vector(orignal, other, index);
    for(uint64_t i = 0; i < orignal.size(); i++) {
        check(orignal[i], actual_vec[i]);
    }
    orignal = { "a", "b", "c" };
    other = { "x", "y", "z" };
    index = 1;
    actual_vec = { "a", "x", "y", "z", "b", "c" };
    preproc::squash_vector(orignal, other, index, false);
    for(uint64_t i = 0; i < orignal.size(); i++) {
        check(orignal[i], actual_vec[i]);
    }
    progress();

    title("preproc::remove_comments()");
    text = "stdout ; Displays the value of Pi. ; 3.14";
    actual = "stdout                               3.14";
    found = text;
    preproc::remove_comments(found);
    check(found, actual);

    text = "stdout 3.14 ;; this whole part will be ignored till line break";
    actual = "stdout 3.14                                                   ";
    found = text;
    preproc::remove_comments(found);
    check(found, actual);

    text = "var name = \"Kal\", ;; multi line comments work till they reach a line break\n    type = \"Language\"";
    actual = "var name = \"Kal\",                                                         \n    type = \"Language\"";
    found = text;
    preproc::remove_comments(found);
    check(found, actual);

    progress();
}