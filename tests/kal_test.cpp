#include <iostream>
#include <vector>

#include "lib_kal_test.hpp"
#include "test_arg_parser.hpp"
#include "test_lexer.hpp"
#include "test_preprocessor.hpp"
#include "test_expr_parser.hpp"
#include "test_parser.hpp"
#include "test_var.hpp"
#include "test_control_flow.hpp"
#include "test_fn.hpp"

int main() {
    test_arg_parser();
    test_preprocessor();
    test_lexer();
    test_expr_parser();
    test_parser();
    test_var();
    test_control_flow();
    test_shadowing();
    test_fn();

    exit_successfully();
    return 0;
}