#pragma once

#include "../var.hpp"

#define SET(X, Y) VarTable::set(X, Y, nullptr, VAR, false, 0, false, globals)

void test_var() {
    double actual_value, found_value;
    Value* actual_ptr;
    Value* found_ptr;
    std::string actual_string, found_string;
    std::vector<Token> tokens;
    std::vector<std::string> lines;

    component("Variable Table");

    // Tests for assignment of simple values.
    title("VarTable::set()");
    SET("name", "\"Kal\"");
    actual_string = "\"Kal\"";
    found_string = VarTable::print("name", globals);
    check(found_string, actual_string);

    SET("pi", "3.14");
    actual_value = 3.14;
    found_value = std::stod(VarTable::print("pi", globals));
    check(found_value, actual_value);

    // Tests for re-assignment of values.
    SET("age", "20");
    actual_value = 20;
    found_value = std::stod(VarTable::print("age", globals));
    check(found_value, actual_value);
    SET("age", "21");
    actual_value = 21;
    found_value = std::stod(VarTable::print("age", globals));
    check(found_value, actual_value);
    SET("age", "\"twenty-one\"");
    actual_string = "\"twenty-one\"";
    found_string = VarTable::print("age", globals);
    check(found_string, actual_string);

    // Test for re-assignment of variables.
    SET("value", "100");
    SET("duplicate", "value");
    actual_value = 100;
    found_value = std::stod(VarTable::print("duplicate", globals));
    check(found_value, actual_value);

    // Tests for assignment of lists.
    SET("data", "[3.14, 9.8, 2.71]");
    actual_string = "[3.14, 9.8, 2.71]";
    found_string = VarTable::print("data", globals);
    check(found_string, actual_string);

    check(std::stod(VarTable::print("data[0]", globals)), 3.14);
    check(std::stod(VarTable::print("data[1]", globals)), 9.8);
    check(std::stod(VarTable::print("data[2]", globals)), 2.71);

    // Test for re-assignment of list items.
    SET("data[1]", "\"g\"");
    actual_string = "[3.14, \"g\", 2.71]";
    found_string = VarTable::print("data", globals);
    check(found_string, actual_string);
    
    actual_string = "\"g\"";
    found_string = VarTable::print("data[1]", globals);
    check(found_string, actual_string);

    // Tests for list of dynamic types.
    SET("types", "[3.14, \"PI\"]");
    actual_value = 3.14;
    found_value = std::stod(VarTable::print("types[0]", globals));
    check(found_value, actual_value);
    actual_string = "\"PI\"";
    found_string = VarTable::print("types[1]", globals);
    check(found_string, actual_string);

    // Tests for assignment of multi-dimensional lists.
    SET("nd_data", "[\n\t[0, 0], \n\t[0, 1], \n\t[1, 0], \n\t[1, 1]\n]");
    actual_string = "[[0, 0], [0, 1], [1, 0], [1, 1]]";
    found_string = VarTable::print("nd_data", globals);
    check(found_string, actual_string);
    actual_string = "[1, 0]";
    // error when nd_data was mis-spelt, fix that.
    found_string = VarTable::print("nd_data[2]", globals);
    check(found_string, actual_string);
    actual_value = 0;
    found_value = std::stod(VarTable::print("nd_data[1][0]", globals));
    check(found_value, actual_value);
    actual_value = 1;
    found_value = std::stod(VarTable::print("nd_data[1][1]", globals));
    check(found_value, actual_value);

    // Tests for re-assignment of lists and multi-dimensional list items.
    SET("nd_data[0]", "\"first\"");
    SET("nd_data[2][0]", "200");
    SET("nd_data[2][1]", "100");
    SET("nd_data[3]", "[\"K\", \"a\", \"l\"]");
    actual_string = "[\"first\", [0, 1], [200, 100], [\"K\", \"a\", \"l\"]]";
    found_string = VarTable::print("nd_data", globals);
    check(found_string, actual_string);

    // Tests for variable assigment from list items.
    SET("a_val", "nd_data[3][1]");
    actual_string = "\"a\"";
    found_string = VarTable::print("a_val", globals);
    check(found_string, actual_string);
    SET("kal_list", "nd_data[3]");
    actual_string = "[\"K\", \"a\", \"l\"]";
    found_string = VarTable::print("kal_list", globals);
    check(found_string, actual_string);

    // Tests for list item to list item assignment.
    SET("first", "[200, [150, 300], 400, 250]");
    SET("second", "[first[1][0], first[0], first[3], first[1][1], first[2]]");
    actual_string = "[150, 200, 250, 300, 400]";
    found_string = VarTable::print("second", globals);
    check(found_string, actual_string);

    // make something like this work
    // SET("new", "$second[$first[1][1] / 100]"); => 300

    // Tests for assignment of dictionaries.
    SET("my_data", "#(name -> \"Clark Kent\", age -> 25, \"place of origin\" -> \"Krypton\")");
    actual_string = "#(name -> \"Clark Kent\", age -> 25, place of origin -> \"Krypton\")";
    found_string = VarTable::print("my_data", globals);
    check(found_string, actual_string);

    SET("bio", "#(jobs -> 9, company -> \"Kal\", positions -> [\"engineering\", \"testing\"])");
    actual_value = 9;
    // add error when wrong key (non-string) is passed.
    found_value = std::stod(VarTable::print("bio[\"jobs\"]", globals));
    check(found_value, actual_value);
    actual_string = "\"engineering\"";
    found_string = VarTable::print("bio[\"positions\"][0]", globals);
    check(found_string, actual_string);

    // Tests for re-assignment of dictionary items.
    SET("bio[\"jobs\"]", "10");
    actual_value = 10;
    found_value = std::stod(VarTable::print("bio[\"jobs\"]", globals));
    check(found_value, actual_value);

    // Tests for variable assignment from dictionary.
    SET("bio_name", "bio[\"company\"]");
    actual_string = "\"Kal\"";
    found_string = VarTable::print("bio_name", globals);
    check(found_string, actual_string);

    SET("bio_post", "bio[\"positions\"][1]");
    actual_string = "\"testing\"";
    found_string = VarTable::print("bio_post", globals);
    check(found_string, actual_string);

    // Tests for nested dictionaries.
    SET("info", "#(name -> \"Superman\", age -> 25, \"real name\" -> #(krypton -> \"Kal-El\", earth -> \"Clark Kent\",))");
    actual_string = "#(krypton -> \"Kal-El\", earth -> \"Clark Kent\")";
    found_string = VarTable::print("info[\"real name\"]", globals);
    check(found_string, actual_string);

    // Tests for dictionary item to dictionary item assigment.
    // Will write later when the eval issue is solved.
    globals.depth = 0;
    VarTable::gc(globals);

    // Tests for references.
    SET("value", "100");
    SET("ref_value", "&value");
    actual_value = 100;
    found_value = std::stod(VarTable::print("ref_value", globals));
    check(found_value, actual_value);

    // Tests to check if reference changes with in variable re-assignment.
    SET("value", "200");
    actual_value = 200;
    found_value = std::stod(VarTable::print("ref_value", globals));
    check(found_value, actual_value);

    // Tests to check if variable changes with reference re-assignment. 
    SET("ref_value", "300");
    actual_value = 300;
    found_value = std::stod(VarTable::print("value", globals));
    check(found_value, actual_value);

    SET("x", "2");
    SET("y", "&x");
    SET("data", "[1, y, 3]");
    actual_string = "[1, 2, 3]";
    found_string = VarTable::print("data", globals);
    check(found_string, actual_string);

    SET("third", "&data[2]");
    SET("third", "300");
    actual_string = "[1, 2, 300]";
    found_string = VarTable::print("data", globals);
    check(found_string, actual_string);

    SET("list", "[5, 10, 15]");
    SET("ref_list", "&list");
    SET("ref_list[1]", "50");
    actual_string = "[5, 50, 15]";
    found_string = VarTable::print("list", globals);
    check(found_string, actual_string);

    SET("list[2]", "500");
    actual_string = "[5, 50, 500]";
    found_string = VarTable::print("ref_list", globals);
    check(found_string, actual_string);

    SET("infoA", "[1, 3, 6]");
    SET("infoB", "[&infoA[2], 7, 9]");
    SET("infoB[0]", "5");
    actual_string = "[1, 3, 5]";
    found_string = VarTable::print("infoA", globals);
    check(found_string, actual_string);
    actual_string = "[5, 7, 9]";
    found_string = VarTable::print("infoB", globals);
    check(found_string, actual_string);

    SET("matrix", "[[1, 2, 3], [4, 5, 6], [7, 8, 9]]");
    SET("duplicate_matrix", "[[1, [&matrix[0][1]], 3], [4, [&matrix[1][1]], 6], [7, [&matrix[2][1]], 9]]");
    SET("duplicate_matrix[0][1][0]", "100");
    SET("duplicate_matrix[1][1][0]", "200");
    SET("duplicate_matrix[2][1][0]", "300");
    actual_string = "[[1, [100], 3], [4, [200], 6], [7, [300], 9]]";
    found_string = VarTable::print("duplicate_matrix", globals);
    check(found_string, actual_string);
    actual_string = "[[1, 100, 3], [4, 200, 6], [7, 300, 9]]";
    found_string = VarTable::print("matrix", globals);
    check(found_string, actual_string);

    globals.depth = 0;
    VarTable::gc(globals);
    SET("data", "#(name -> \"Superman\")");
    SET("name", "&data[\"name\"]");
    SET("name", "\"Clark\"");
    actual_string = "#(name -> \"Clark\")";
    found_string = VarTable::print("data", globals);
    check(found_string, actual_string);
    globals.depth = 0;
    VarTable::gc(globals);

    SET("full_name", "#(name -> [\"Clark\", \"Kent\"])");
    SET("first_name", "&full_name[\"name\"][0]");
    SET("first_name", "\"Jon\"");
    actual_string = "#(name -> [\"Jon\", \"Kent\"])";
    found_string = VarTable::print("full_name", globals);
    check(found_string, actual_string);

    SET("info", "#(alias -> \"Superman\", name -> #(first -> \"xyz\", last -> \"Kent\"))");
    SET("ref_name", "&info[\"name\"][\"first\"]");
    SET("ref_name", "\"Clark\"");
    actual_string = "#(first -> \"Clark\", last -> \"Kent\")";
    found_string = VarTable::print("info[\"name\"]", globals);
    check(found_string, actual_string);
    globals.depth = 0;
    VarTable::gc(globals);

    // Tests for unpacking / destructuring.
    // Testing unpacking from Lists.
    SET("[a, b, c]", "[1, 2, 3]");
    actual_string = "1";
    found_string = VarTable::print("a", globals);
    check(found_string, actual_string);
    actual_string = "2";
    found_string = VarTable::print("b", globals);
    check(found_string, actual_string);
    actual_string = "3";
    found_string = VarTable::print("c", globals);
    check(found_string, actual_string);

    SET("[x, y]", "[45, 55, 100]");
    actual_string = "45";
    found_string = VarTable::print("x", globals);
    check(found_string, actual_string);
    actual_string = "55";
    found_string = VarTable::print("y", globals);
    check(found_string, actual_string);

    SET("[p, _, r]", "[\"Hello\", \"There\", \"World\"]");
    actual_string = "\"Hello\"";
    found_string = VarTable::print("p", globals);
    check(found_string, actual_string);
    actual_string = "\"World\"";
    found_string = VarTable::print("r", globals);
    check(found_string, actual_string);

    SET("data", "[3.14, 9.8, 2.71]");
    SET("[_, g, e]", "data");
    actual_string = "9.8";
    found_string = VarTable::print("g", globals);
    check(found_string, actual_string);
    found_string = "2.71";
    actual_string = VarTable::print("e", globals);
    check(found_string, actual_string);
    globals.depth = 0;
    VarTable::gc(globals);

    // Tests for nested List unpacking.
    SET("[a, _, [x, y]]", "[1, 2, [3, 4]]");
    actual_string = "1";
    found_string = VarTable::print("a", globals);
    check(found_string, actual_string);
    actual_string = "3";
    found_string = VarTable::print("x", globals);
    check(found_string, actual_string);
    actual_string = "4";
    found_string = VarTable::print("y", globals);
    check(found_string, actual_string);
    globals.depth = 0;
    VarTable::gc(globals);
    SET("[_, _, [_, [a, _, b]]]", "[1, 2, [\"Hello\", [25, 100, 75]]]");
    actual_string = "25";
    found_string = VarTable::print("a", globals);
    check(found_string, actual_string);
    actual_string = "75";
    found_string = VarTable::print("b", globals);
    check(found_string, actual_string);

    // Tests for swapping two variables using unpacking.
    SET("first", "10");
    SET("second", "20");
    SET("[first, second]", "[second, first]");
    actual_string = "20";
    found_string = VarTable::print("first", globals);
    check(found_string, actual_string);
    actual_string = "10";
    found_string = VarTable::print("second", globals);
    check(found_string, actual_string);
    globals.depth = 0;
    VarTable::gc(globals);

    // Tests for unpacking from Dictionaries.
    SET("#(name, age)", "#(name -> \"Kal\", age -> 25)");
    actual_string = "\"Kal\"";
    found_string = VarTable::print("name", globals);
    check(found_string, actual_string);

    SET("#(b, a)", "#(a -> 10, b -> 20)");
    actual_string = "10";
    found_string = VarTable::print("a", globals);
    check(found_string, actual_string);
    actual_string = "20";
    found_string = VarTable::print("b", globals);
    check(found_string, actual_string);

    SET("data", "#(pi -> 3.14, g -> 9.8, e -> 2.71)");
    SET("#(g, pi)", "data");
    actual_string = "9.8";
    found_string = VarTable::print("g", globals);
    check(found_string, actual_string);
    actual_string = "3.14";
    found_string = VarTable::print("pi", globals);
    check(found_string, actual_string);
    globals.depth = 0;
    VarTable::gc(globals);

    // Test for upacking Dictionaries nested into Lists.
    SET("[_, a, #(b, c)]", "[1, 2, #(b -> \"B\", c -> \"C\")]");
    actual_string = "2";
    found_string = VarTable::print("a", globals);
    check(found_string, actual_string);
    actual_string = "\"B\"";
    found_string = VarTable::print("b", globals);
    check(found_string, actual_string);
    actual_string = "\"C\"";
    found_string = VarTable::print("c", globals);
    check(found_string, actual_string);
    SET("[_, #(p, q, r), _]", "[1, #(r -> 10, p -> [2, 2, 5], q -> 30), 3]");
    actual_string = "10";
    found_string = VarTable::print("r", globals);
    check(found_string, actual_string);
    actual_string = "30";
    found_string = VarTable::print("q", globals);
    check(found_string, actual_string);
    actual_string = "[2, 2, 5]";
    found_string = VarTable::print("p", globals);
    check(found_string, actual_string);

    // Tests for the walrus (:=) operator.
    SET("new_data", "new_info := 100");
    actual_string = "100";
    found_string = VarTable::print("new_info", globals);
    check(found_string, actual_string);
    found_string = VarTable::print("new_data", globals);
    check(found_string, actual_string);

    SET("new_data", "new_info_a := (45 + 55)");
    actual_string = "100";
    found_string = VarTable::print("new_info_a", globals);
    check(found_string, actual_string);
    found_string = VarTable::print("new_data", globals);
    check(found_string, actual_string);

    SET("new_data", "(new_info_b := 100) + 100");
    actual_string = "100";
    found_string = VarTable::print("new_info_b", globals);
    check(found_string, actual_string);
    actual_string = "200";
    found_string = VarTable::print("new_data", globals);
    check(found_string, actual_string);

    SET("xyz", "50");
    SET("abc", "pqr := &xyz");
    actual_string = "50";
    found_string = VarTable::print("pqr", globals);
    check(found_string, actual_string);
    found_string = VarTable::print("abc", globals);
    check(found_string, actual_string);

    SET("abc", "10");
    actual_string = "10";
    found_string = VarTable::print("pqr", globals);
    check(found_string, actual_string);

    SET("pqr", "20");
    actual_string = "20";
    found_string = VarTable::print("abc", globals);
    check(found_string, actual_string);

    progress();

    // Test for performing Garbage Collection (GC).
    title("VarTable::gc()");
    globals.depth = 0;
    VarTable::gc(globals);
    progress();

    // Tests for unnames scopes.
    title("Unnamed Scopes");
    lines = {
        "var value = 10",
        "{",
            "var info = 20",
        "}"
    };
    tokens = lexer::tokenize(lines);
    line_exec(tokens, false, false, false, globals);
    found_string = VarTable::print("value", globals);
    actual_string = "10";
    check(found_string, actual_string);
    found_ptr = VarTable::get("info", {}, true, true, true, globals);
    actual_ptr = nullptr;
    check(found_ptr, actual_ptr);

    lines = {
        "value = 0",
        "{",
            "var data = 20",
            "value = data",
        "}"
    };
    tokens = lexer::tokenize(lines);
    line_exec(tokens, false, false, false, globals);
    found_string = VarTable::print("value", globals);
    actual_string = "20",
    check(found_string, actual_string);
    found_ptr = VarTable::get("data", {}, true, true, true, globals);
    actual_ptr = nullptr;
    check(found_ptr, actual_ptr);

    lines = {
        "value = 0",
        "var temp = 100",
        "{",
            "var temp = 10",
            "value = value + temp",
            "{",
                "var temp = 20",
                "value = value + temp",
                "{",
                    "var temp = 30",
                    "value = value + temp",
                "}",
            "}",
        "}"
    };
    tokens = lexer::tokenize(lines);
    line_exec(tokens, false, false, false, globals);
    found_string = VarTable::print("temp", globals);
    actual_string = "100";
    check(found_string, actual_string);
    found_string = VarTable::print("value", globals);
    actual_string = "60";
    check(found_string, actual_string);

    VarTable::gc(globals);
    progress();
}