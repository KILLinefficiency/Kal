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
    progress();

    title("preproc::clean_contents()");
    orignal = { " a", "\tb", "\nc", "a ", "b\t", "c\n", " a ", "\tb\t", "\nc\n", "  \t\t\n abc \n\t\t  " };
    actual_vec= { "a", "b", "c", "a", "b", "c", "a", "b", "c", "abc" };
    found_vec = preproc::clean_contents(orignal);
    for(uint64_t i = 0; i < orignal.size(); i++) {
        check(found_vec[i], actual_vec[i]);
    }
    progress();
}