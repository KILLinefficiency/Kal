#pragma once

#include "../var.hpp"

void test_var() {
    double actual_value, found_value;
    std::string actual_string, found_string;

    component("Variable Table");

    // Tests for assignment of simple values.
    title("VarTable::set()");
    VarTable::set("name", "\"Kal\"");
    actual_string = "\"Kal\"";
    found_string = VarTable::print("$name");
    check(found_string, actual_string);

    VarTable::set("pi", "3.14");
    actual_value = 3.14;
    found_value = std::stod(VarTable::print("$pi"));
    check(found_value, actual_value);

    // Tests for re-assignment of values.
    VarTable::set("age", "20");
    actual_value = 20;
    found_value = std::stod(VarTable::print("$age"));
    check(found_value, actual_value);
    VarTable::set("$age", "21");
    actual_value = 21;
    found_value = std::stod(VarTable::print("$age"));
    check(found_value, actual_value);
    VarTable::set("$age", "\"twenty-one\"");
    actual_string = "\"twenty-one\"";
    found_string = VarTable::print("$age");
    check(found_string, actual_string);

    // Test for re-assignment of variables.
    VarTable::set("value", "100");
    VarTable::set("duplicate", "$value");
    actual_value = 100;
    found_value = std::stod(VarTable::print("$duplicate"));
    check(found_value, actual_value);

    // Tests for assignment of lists.
    VarTable::set("data", "[3.14, 9.8, 2.71]");
    actual_string = "[3.14, 9.8, 2.71]";
    found_string = VarTable::print("$data");
    check(found_string, actual_string);

    check(std::stod(VarTable::print("$data[0]")), 3.14);
    check(std::stod(VarTable::print("$data[1]")), 9.8);
    check(std::stod(VarTable::print("$data[2]")), 2.71);

    // Test for re-assignment of list items.
    VarTable::set("$data[1]", "\"g\"");
    actual_string = "[3.14, \"g\", 2.71]";
    found_string = VarTable::print("$data");
    check(found_string, actual_string);
    
    actual_string = "\"g\"";
    found_string = VarTable::print("$data[1]");
    check(found_string, actual_string);

    // Tests for list of dynamic types.
    VarTable::set("types", "[3.14, \"PI\"]");
    actual_value = 3.14;
    found_value = std::stod(VarTable::print("$types[0]"));
    check(found_value, actual_value);
    actual_string = "\"PI\"";
    found_string = VarTable::print("$types[1]");
    check(found_string, actual_string);

    // Tests for assignment of multi-dimensional lists.
    VarTable::set("nd_data", "[\n\t[0, 0], \n\t[0, 1], \n\t[1, 0], \n\t[1, 1]\n]");
    actual_string = "[[0, 0], [0, 1], [1, 0], [1, 1]]";
    found_string = VarTable::print("$nd_data");
    check(found_string, actual_string);
    actual_string = "[1, 0]";
    // error when nd_data was mis-spelt, fix that.
    found_string = VarTable::print("$nd_data[2]");
    check(found_string, actual_string);
    actual_value = 0;
    found_value = std::stod(VarTable::print("$nd_data[1][0]"));
    check(found_value, actual_value);
    actual_value = 1;
    found_value = std::stod(VarTable::print("$nd_data[1][1]"));
    check(found_value, actual_value);

    // Tests for re-assignment of lists and multi-dimensional list items.
    VarTable::set("$nd_data[0]", "\"first\"");
    VarTable::set("$nd_data[2][0]", "200");
    VarTable::set("$nd_data[2][1]", "100");
    VarTable::set("$nd_data[3]", "[\"K\", \"a\", \"l\"]");
    actual_string = "[\"first\", [0, 1], [200, 100], [\"K\", \"a\", \"l\"]]";
    found_string = VarTable::print("$nd_data");
    check(found_string, actual_string);

    // Tests for variable assigment from list items.
    VarTable::set("a_val", "$nd_data[3][1]");
    actual_string = "\"a\"";
    found_string = VarTable::print("$a_val");
    check(found_string, actual_string);
    VarTable::set("kal_list", "$nd_data[3]");
    actual_string = "[\"K\", \"a\", \"l\"]";
    found_string = VarTable::print("$kal_list");
    check(found_string, actual_string);

    // Tests for list item to list item assignment.
    VarTable::set("first", "[200, [150, 300], 400, 250]");
    VarTable::set("second", "[$first[1][0], $first[0], $first[3], $first[1][1], $first[2]]");
    actual_string = "[150, 200, 250, 300, 400]";
    found_string = VarTable::print("$second");
    check(found_string, actual_string);

    // make something like this work
    // VarTable::set("new", "$second[$first[1][1] / 100]"); => 300

    // Tests for assignment of dictionaries.
    VarTable::set("my_data", "#(name -> \"Clark Kent\", age -> 25, \"place of origin\" -> \"Krypton\")");
    actual_string = "#(name -> \"Clark Kent\", age -> 25, place of origin -> \"Krypton\")";
    found_string = VarTable::print("$my_data");
    check(found_string, actual_string);

    VarTable::set("bio", "#(jobs -> 9, company -> \"Kal\", positions -> [\"engineering\", \"testing\"])");
    actual_value = 9;
    // add error when wrong key (non-string) is passed.
    found_value = std::stod(VarTable::print("$bio[\"jobs\"]"));
    check(found_value, actual_value);
    actual_string = "\"engineering\"";
    found_string = VarTable::print("$bio[\"positions\"][0]");
    check(found_string, actual_string);

    // Tests for re-assignment of dictionary items.
    VarTable::set("$bio[\"jobs\"]", "10");
    actual_value = 10;
    found_value = std::stod(VarTable::print("$bio[\"jobs\"]"));
    check(found_value, actual_value);

    // Tests for variable assignment from dictionary.
    VarTable::set("bio_name", "$bio[\"company\"]");
    actual_string = "\"Kal\"";
    found_string = VarTable::print("$bio_name");
    check(found_string, actual_string);

    VarTable::set("bio_post", "$bio[\"positions\"][1]");
    actual_string = "\"testing\"";
    found_string = VarTable::print("$bio_post");
    check(found_string, actual_string);

    // Tests for nested dictionaries.
    VarTable::set("info", "#(name -> \"Superman\", age -> 25, \"real name\" -> #(krypton -> \"Kal-El\", earth -> \"Clark Kent\",))");
    actual_string = "#(krypton -> \"Kal-El\", earth -> \"Clark Kent\")";
    found_string = VarTable::print("$info[\"real name\"]");
    check(found_string, actual_string);

    // Tests for dictionary item to dictionary item assigment.
    // Will write later when the eval isse is solved.
    VarTable::gc();

    // Tests for references.
    VarTable::set("value", "100");
    VarTable::set("ref_value", "$&value");
    actual_value = 100;
    found_value = std::stod(VarTable::print("$ref_value"));
    check(found_value, actual_value);

    // Tests to check if reference changes with in variable re-assignment.
    VarTable::set("$value", "200");
    actual_value = 200;
    found_value = std::stod(VarTable::print("$ref_value"));
    check(found_value, actual_value);

    // Tests to check if variable changes with reference re-assignment. 
    VarTable::set("$ref_value", "300");
    actual_value = 300;
    found_value = std::stod(VarTable::print("$value"));
    check(found_value, actual_value);

    progress();

    // Test for performing Garbage Collection (GC).
    title("VarTable::gc()");
    VarTable::gc();

    progress();
}