#pragma once

#include "../embed/dynamic_libkal.cpp"

void test_libkal_o() {
    component("libkal.o");

    title("Result()");
    Result null_result = Result("null");
    check(null_result.value, std::string("null"));
    check<bool>(null_result.to_null(), true);

    Result result_number = Result("100");
    check(result_number.value, std::string("100"));
    check<double>(result_number.to_number(), 100.0);

    Result result_string = Result("\"Superman\"");
    check(result_string.value, std::string("\"Superman\""));
    check(result_string.to_string(), std::string("Superman"));

    Result result_list = Result("[1, \"Hello\", 2]");
    check(result_list.value, std::string("[1, \"Hello\", 2]"));

    check(result_list[0].value, std::string("1"));
    check<double>(result_list[0].to_number(), 1.0);
    check(result_list[1].value, std::string("\"Hello\""));
    check(result_list[1].to_string(), std::string("Hello"));
    check(result_list[2].value, std::string("2"));
    check<double>(result_list[2].to_number(), 2.0);

    Result result_nested_list = Result("[[1, 2], [3, 4]]");
    check(result_nested_list.value, std::string("[[1, 2], [3, 4]]"));

    check(result_nested_list[0].value, std::string("[1, 2]"));
    std::vector<Result> first_list = result_nested_list[0].to_list();
    check(first_list[0].value, std::string("1"));
    check<double>(first_list[0].to_number(), 1.0);
    check(first_list[1].value, std::string("2"));
    check<double>(first_list[1].to_number(), 2.0);
    check(result_nested_list[1].value, std::string("[3, 4]"));
    std::vector<Result> second_list = result_nested_list[1].to_list();
    check(second_list[0].value, std::string("3"));
    check<double>(second_list[0].to_number(), 3.0);
    check(second_list[1].value, std::string("4"));
    check<double>(second_list[1].to_number(), 4.0);
    check(result_nested_list[0][0].value, std::string("1"));
    check<double>(result_nested_list[0][0].to_number(), 1.0);
    check(result_nested_list[0][1].value, std::string("2"));
    check<double>(result_nested_list[0][1].to_number(), 2.0);
    check(result_nested_list[1][0].value, std::string("3"));
    check<double>(result_nested_list[1][0].to_number(), 3.0);
    check(result_nested_list[1][1].value, std::string("4"));
    check<double>(result_nested_list[1][1].to_number(), 4.0);

    Result result_dict = Result("#(name -> \"Superman\", age -> 25)");
    check(result_dict.value, std::string("#(name -> \"Superman\", age -> 25)"));
    check(result_dict["name"].value, std::string("\"Superman\""));
    check(result_dict["name"].to_string(), std::string("Superman"));
    check(result_dict["age"].value, std::string("25"));
    check<double>(result_dict["age"].to_number(), 25.0);

    progress();

    title("Kal::exec()");
    progress();
}