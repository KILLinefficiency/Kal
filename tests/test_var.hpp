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
    // Will write later when the eval issue is solved.
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

    VarTable::set("x", "2");
    VarTable::set("y", "$&x");
    VarTable::set("data", "[1, $y, 3]");
    actual_string = "[1, 2, 3]";
    found_string = VarTable::print("$data");
    check(found_string, actual_string);

    VarTable::set("third", "$&data[2]");
    VarTable::set("$third", "300");
    actual_string = "[1, 2, 300]";
    found_string = VarTable::print("$data");
    check(found_string, actual_string);

    VarTable::set("list", "[5, 10, 15]");
    VarTable::set("ref_list", "$&list");
    VarTable::set("$ref_list[1]", "50");
    actual_string = "[5, 50, 15]";
    found_string = VarTable::print("$list");
    check(found_string, actual_string);

    VarTable::set("$list[2]", "500");
    actual_string = "[5, 50, 500]";
    found_string = VarTable::print("$ref_list");
    check(found_string, actual_string);

    VarTable::set("infoA", "[1, 3, 6]");
    VarTable::set("infoB", "[$&infoA[2], 7, 9]");
    VarTable::set("$infoB[0]", "5");
    actual_string = "[1, 3, 5]";
    found_string = VarTable::print("$infoA");
    check(found_string, actual_string);
    actual_string = "[5, 7, 9]";
    found_string = VarTable::print("$infoB");
    check(found_string, actual_string);

    VarTable::set("matrix", "[[1, 2, 3], [4, 5, 6], [7, 8, 9]]");
    VarTable::set("duplicate_matrix", "[[1, [$&matrix[0][1]], 3], [4, [$&matrix[1][1]], 6], [7, [$&matrix[2][1]], 9]]");
    VarTable::set("$duplicate_matrix[0][1][0]", "100");
    VarTable::set("$duplicate_matrix[1][1][0]", "200");
    VarTable::set("$duplicate_matrix[2][1][0]", "300");
    actual_string = "[[1, [100], 3], [4, [200], 6], [7, [300], 9]]";
    found_string = VarTable::print("$duplicate_matrix");
    check(found_string, actual_string);
    actual_string = "[[1, 100, 3], [4, 200, 6], [7, 300, 9]]";
    found_string = VarTable::print("$matrix");
    check(found_string, actual_string);

    VarTable::set("data", "#(name -> \"Superman\")");
    VarTable::set("name", "$&data[\"name\"]");
    VarTable::set("$name", "\"Clark\"");
    actual_string = "#(name -> \"Clark\")";
    found_string = VarTable::print("$data");
    check(found_string, actual_string);

    VarTable::set("full_name", "#(name -> [\"Clark\", \"Kent\"])");
    VarTable::set("first_name", "$&full_name[\"name\"][0]");
    VarTable::set("$first_name", "\"Jon\"");
    actual_string = "#(name -> [\"Jon\", \"Kent\"])";
    found_string = VarTable::print("$full_name");
    check(found_string, actual_string);

    VarTable::set("info", "#(alias -> \"Superman\", name -> #(first -> \"xyz\", last -> \"Kent\"))");
    VarTable::set("ref_name", "$&info[\"name\"][\"first\"]");
    VarTable::set("$ref_name", "\"Clark\"");
    actual_string = "#(first -> \"Clark\", last -> \"Kent\")";
    found_string = VarTable::print("$info[\"name\"]");
    check(found_string, actual_string);
    VarTable::gc();

    // Tests for unpacking / destructuring.
    // Testing unpacking from Lists.
    VarTable::set("[a, b, c]", "[1, 2, 3]");
    actual_string = "1";
    found_string = VarTable::print("$a");
    check(found_string, actual_string);
    actual_string = "2";
    found_string = VarTable::print("$b");
    check(found_string, actual_string);
    actual_string = "3";
    found_string = VarTable::print("$c");
    check(found_string, actual_string);

    VarTable::set("[x, y]", "[45, 55, 100]");
    actual_string = "45";
    found_string = VarTable::print("$x");
    check(found_string, actual_string);
    actual_string = "55";
    found_string = VarTable::print("$y");
    check(found_string, actual_string);

    VarTable::set("[p, _, r]", "[\"Hello\", \"There\", \"World\"]");
    actual_string = "\"Hello\"";
    found_string = VarTable::print("$p");
    check(found_string, actual_string);
    actual_string = "\"World\"";
    found_string = VarTable::print("$r");
    check(found_string, actual_string);

    VarTable::set("data", "[3.14, 9.8, 2.71]");
    VarTable::set("[_, g, e]", "$data");
    actual_string = "9.8";
    found_string = VarTable::print("$g");
    check(found_string, actual_string);
    found_string = "2.71";
    actual_string = VarTable::print("$e");
    check(found_string, actual_string);
    VarTable::gc();

    // Tests for nested List unpacking.
    VarTable::set("[a, _, [x, y]]", "[1, 2, [3, 4]]");
    actual_string = "1";
    found_string = VarTable::print("$a");
    check(found_string, actual_string);
    actual_string = "3";
    found_string = VarTable::print("$x");
    check(found_string, actual_string);
    actual_string = "4";
    found_string = VarTable::print("$y");
    check(found_string, actual_string);
    VarTable::gc();
    VarTable::set("[_, _, [_, [a, _, b]]]", "[1, 2, [\"Hello\", [25, 100, 75]]]");
    actual_string = "25";
    found_string = VarTable::print("$a");
    check(found_string, actual_string);
    actual_string = "75";
    found_string = VarTable::print("$b");
    check(found_string, actual_string);

    // Tests for swapping two variables using unpacking.
    VarTable::set("first", "10");
    VarTable::set("second", "20");
    VarTable::set("[first, second]", "[$second, $first]");
    actual_string = "20";
    found_string = VarTable::print("$first");
    check(found_string, actual_string);
    actual_string = "10";
    found_string = VarTable::print("$second");
    check(found_string, actual_string);
    VarTable::gc();

    // Tests for unpacking from Dictionaries.
    VarTable::set("#(name, age)", "#(name -> \"Kal\", age -> 25)");
    actual_string = "\"Kal\"";
    found_string = VarTable::print("$name");
    check(found_string, actual_string);

    VarTable::set("#(b, a)", "#(a -> 10, b -> 20)");
    actual_string = "10";
    found_string = VarTable::print("$a");
    check(found_string, actual_string);
    actual_string = "20";
    found_string = VarTable::print("$b");
    check(found_string, actual_string);

    VarTable::set("data", "#(pi -> 3.14, g -> 9.8, e -> 2.71)");
    VarTable::set("#(g, pi)", "$data");
    actual_string = "9.8";
    found_string = VarTable::print("$g");
    check(found_string, actual_string);
    actual_string = "3.14";
    found_string = VarTable::print("$pi");
    check(found_string, actual_string);
    VarTable::gc();

    // Test for upacking Dictionaries nested into Lists.
    VarTable::set("[_, a, #(b, c)]", "[1, 2, #(b -> \"B\", c -> \"C\")]");
    actual_string = "2";
    found_string = VarTable::print("$a");
    check(found_string, actual_string);
    actual_string = "\"B\"";
    found_string = VarTable::print("$b");
    check(found_string, actual_string);
    actual_string = "\"C\"";
    found_string = VarTable::print("$c");
    check(found_string, actual_string);
    VarTable::set("[_, #(p, q, r), _]", "[1, #(r -> 10, p -> [2, 2, 5], q -> 30), 3]");
    actual_string = "10";
    found_string = VarTable::print("$r");
    check(found_string, actual_string);
    actual_string = "30";
    found_string = VarTable::print("$q");
    check(found_string, actual_string);
    actual_string = "[2, 2, 5]";
    found_string = VarTable::print("$p");
    check(found_string, actual_string);

    progress();

    // Test for performing Garbage Collection (GC).
    title("VarTable::gc()");
    VarTable::gc();

    progress();
}