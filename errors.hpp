#pragma once

#include <sstream>
#include <vector>
#include <cstdint>

#include <stack>
#include <utility>

#include "lib/lib_style.hpp"

#define CALL_STACK std::stack<std::pair<std::string, int>>& call_stack

std::string fmt(std::string body, std::vector<std::string> args) {
    std::stringstream text;
    uint64_t i = 0, count = 0, len = body.size(), size = args.size();

    while(i < len) {
        if(body[i] == '{' && body[i + 1] == '}' && count < size) {
            text << args[count];
            count++;
            i += 2;
            continue;
        }

        if(body[i] == '\n') {
            text << "\n\u2502 ";
            i++;
            continue;
        }

        text << body[i];
        i++;
    }

    return text.str();
}

std::string write_line(std::string& line) {
    std::stringstream display_line;
    display_line << style::style["reset"] << style::style["italic"] << style::style["green"];
    uint64_t i = 0, size = line.size();
    while(i < size) {
        if(line[i] == '\n') {
            display_line << style::style["reset"] << "\n\u2502       ";
            display_line << style::style["italic"] << style::style["green"];
            i++;
            continue;
        }
        display_line << line[i];
        i++;
    }
    return display_line.str();
}

namespace warnings {
    void const_uninitialized_warning(const std::string& const_name) {
        std::cerr << style::style["yellow"] << style::style["bold"] << "Variable:" << style::style["reset"] << style::style["yellow"] << " Uninitialized constant(s) `" << const_name << "` won't accept assignment later." << style::style["reset"] << std::endl;
        exit(2);
    }
}

namespace errors {
    void throw_err(Globals& globals, std::string head, std::string body, std::initializer_list<std::string> list = {}) {
        std::vector<std::string> args(list);
        std::string blank = "-";
        for(std::string& each : args) {
            each = style::style["bold"] + style::style["yellow"] + each + style::style["reset"];
        }
        std::cerr << "\n\u250C\u2500\n\u2502 " << style::style["red"] << style::style["bold"] << style::style["underline"] << head << " Error" << style::style["reset"] << "\n\u2502 "
            << style::style["green"] << style::style["bold"] << "Line: " << write_line(globals.current_line != nullptr ? *globals.current_line : blank) << style::style["reset"] << "\n\u2502 "
            << fmt(body, args) << "\n\u2514\u2500\n" << std::endl; 
        if(globals.call_stack.size() != 0) {
            std::cerr << "\u2502\n\u2502 " << style::style["bold"] << "Call Stack" << style::style["reset"] << "\n";
            while(globals.call_stack.size() != 0) {
                std::cerr << "\u2502 " << globals.call_stack.top().first << "\n";
                globals.call_stack.pop();
            }
            std::cerr << "\u2502\n";
        }
        if(globals.error_exit) exit(1);
    }

    void throw_parser_error(std::string error_message, const std::string& line) {
        std::cerr << "\n\u250C\u2500\n\u2502 " << style::style["red"] << style::style["bold"] << style::style["underline"] << "Parser Error" << style::style["reset"] << "\n\u2502 "
            << style::style["green"] << style::style["bold"] << "Line: " << line << style::style["reset"] << "\n\u2502 "  
            << "Error: " << error_message << "\n\u2514\u2500" << std::endl;
        exit(1);
    }

    void missing_operands(const std::string& line) {
        throw_parser_error("Missing Operators.", line);
    }

    void access_commas(const std::string& line) {
        throw_parser_error("Access cannot contain commas.", line);
    }

    void only_primitive_access(const std::string& line) {
        throw_parser_error("Only primitive values allowed for access.", line);
    }

    void list_eol(const std::string& line) {
        throw_parser_error("Unclosed List.", line);
    }

    void invalid_assignment(const std::string& line) {
        throw_parser_error("Invalid Assignment.", line);
    }

    void no_anon_fn(const std::string& line) {
        throw_parser_error("Invalid use of anonymous function.", line);
    }

    void high_loop_segments(const std::string& line) {
        throw_parser_error("Too many loop segments.", line);
    }

    void string_eol(const std::string& line) {
        throw_parser_error("Unclosed String.", line);
    }

    void invalid_expression(const std::string& line) {
        throw_parser_error("Invalid Expression.", line);
    }

    void invalid_operator(const std::string& line) {
        throw_parser_error("Invalid Operator", line);
    }

    void unidentified_keyword(const std::string& line) {
        throw_parser_error("Unidentified Keyword.", line);
    }

    void var_number(const std::string& line) {
        throw_parser_error("Variable name cannot start with a number.", line);
    }

    void invalid_target_op(const std::string& line) {
        throw_parser_error("Invalid use of target operator.", line);
    }

    void no_init_required(const std::string& line) {
        throw_parser_error("Invalid use of variable initialization", line);
    }

    void no_keyword_required(const std::string& line) {
        throw_parser_error("Invalid use of keyword.", line);
    }

    void single_arg_required(const std::string& line) {
        throw_parser_error("Statement takes only single argument.", line);
    }

    void kal_error(std::string kal_err) {
        std::cerr << style::style["red"] << style::style["bold"] << "Kal:" << style::style["reset"] << style::style["red"] << " " << kal_err << style::style["reset"] << std::endl;
        exit(1);
    }

    /*void invalid_operation_error(std::string& token, std::string& a, std::string& b) {
        std::cerr << style::style["red"] << style::style["bold"] << "Expression:" << style::style["reset"] << style::style["red"] << " Invalid Expression: " << a << " " << token << " " << b << ".\nCannot perform " << token << " on " << a << " and " << b << "." << style::style["reset"] << std::endl;
        exit(1);
    }

    void invalid_string_operation_error(std::string& token, std::string& a, std::string& b) {
        throw_err("Expression", "Cannot use operator " + token + " on strings " + a + " and " + b);
    }*/

    void no_cmd_arg(const std::string& line) {
        throw_parser_error("Incomplete command line arguments.", line);
    }

    void unparsable_token(const std::string& line) {
        throw_parser_error("Unparsable Token.", line);
    }

    void closing_scope(const std::string& line) {
        throw_parser_error("Closing { expected.", line);
    }

    void invalid_operation(Globals& globals, std::string type, std::string& op, std::string& val1, std::string& val2) {
        throw_err(globals, "Expression", "Cannot use operator {} on " + type + " {} and {}.", { op, val1, val2 });
    }
    
    // void var_redeclare(std::string var_name, std::string var_type) {
    //     std::cerr << style::style["red"] << style::style["bold"] << "Variable:" << style::style["reset"] << style::style["red"] << " Variable `" << var_name << "` of type `" << var_type << "` already exists." << style::style["reset"] << std::endl;
    //     exit(1);
    // }

    void undefined_var(Globals& globals, std::string& var_name) {
        throw_err(globals, "Variable Undefined", "Variable {} is undefined.", { var_name });
    }

    void cannot_write_to_literal_error(const std::string& literal) {
        std::cerr << style::style["red"] << style::style["bold"] << "Variable: " << style::style["reset"] << style::style["red"] << "Cannot write data to string literal `" << literal << "`." << style::style["reset"] << std::endl;
        exit(1);
    }

    void file_already_included_error(const std::string& file_name) {
        std::cerr << style::style["red"] << style::style["bold"] << "Preprocessor:" << style::style["reset"] << style::style["red"] << " File `" << file_name << "` is already included." << style::style["reset"] << std::endl;
        exit(1);
    }

    void file_included_in_itself_error(const std::string& file_name) {
        std::cerr << style::style["red"] << style::style["bold"] << "Preprocessor:" << style::style["reset"] << style::style["red"] << " Cannot include file `" << file_name << "` in itself." << style::style["reset"] << std::endl;
        exit(1);
    }

    void file_does_not_exist_error(Globals& globals, std::string& file_name) {
        std::string line = "-";
        throw_err(globals, "File Not Found", "File {} not found.", { file_name });
    }

    void unidentified_keyword(Globals& globals, std::string& keyword) {
        throw_err(globals, "Kal Keyword", "Keyword {} is unidentified.", { keyword });
    }

    void index_error(Globals& globals, std::string& index) {
        throw_err(globals, "Index", "Index {} out of bounds.", { index });
    }

    void key_error(Globals& globals, std::string& key) {
        throw_err(globals, "Key", "Key {} does not exist.", { key });
    }

    void invalid_else(Globals& globals) {
        throw_err(globals, "Invalid Control Flow", "Cannot use an {} without a valid {}.", { "else", "if" });
    }

    void undefined_function(Globals& globals, std::string& fn_name) {
        throw_err(globals, "Undefined Function", "Function {} not found.", { fn_name });
    }

    void top_return(Globals& globals) {
        throw_err(globals, "Return", "Cannot return at the top level.");
    }

    void defer_outside_fn(Globals& globals) {
        throw_err(globals, "Defer", "defer cannot be used outside a function.");
    }

    void invalid_ref_assign(Globals& globals) {
        throw_err(globals, "Assignment", "Cannot use & for variable naming.");
    }

    void invalid_continue(Globals& globals) {
        throw_err(globals, "Control Flow", "Invalid use of the continue keyword.");
    }

    void invalid_break(Globals& globals) {
        throw_err(globals, "Control Flow", "Invalid use of the break keyword.");
    }

    void items_unpack(Globals& globals) {
        throw_err(globals, "Unpack", "More than enough items to unpack.");
    }

    void var_redeclare(Globals& globals, const std::string& var_name) {
        throw_err(globals, "Scope", "Variable {} redeclared within same scope.", { var_name });
    }

    void expected_arguments(Globals& globals, const std::string& head, int arg_num) {
        std::string label = arg_num == 1 ? "argument" : "arguments";
        throw_err(globals, "Argument", "Expected {} {} for {}.", { std::to_string(arg_num), label, head });
    }

    void fn_extra_args(Globals& globals, const std::string& fn_name) {
        throw_err(globals, "Argument", "More than expected arguments for function {}.", { fn_name });
    }

    void fn_less_args(Globals& globals, const std::string& fn_name) {
        throw_err(globals, "Argument", "No enough arguments passed to function {}.", { fn_name });
    }

    void invalid_ref(Globals& globals, const std::string& ref_name) {
        throw_err(globals, "Reference", "Dangling reference {} found.", { ref_name });
    }

    void empty_list(Globals& globals, const std::string& list_name) {
        throw_err(globals, "List", "Empty list found {}.", { list_name });
    }
}