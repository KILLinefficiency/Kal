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
    //void throw_err(std::stack<std::pair<std::string, int>>& call_stack, std::string& line, std::string head, std::string body, std::initializer_list<std::string> list = {}, bool quit = true) {
        std::vector<std::string> args(list);
        for(std::string& each : args) {
            each = style::style["bold"] + style::style["yellow"] + each + style::style["reset"];
        }
        std::cerr << "\n\u250C\u2500\n\u2502 " << style::style["red"] << style::style["bold"] << style::style["underline"] << head << " Error" << style::style["reset"] << "\n\u2502 "
            << style::style["green"] << style::style["bold"] << "Line: " << write_line(*globals.current_line) << style::style["reset"] << "\n\u2502 "
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

    void invalid_operation(Globals& globals, std::string type, std::string& op, std::string& val1, std::string& val2) {
        throw_err(globals, "Expression", "Cannot use operator {} on " + type + " {} and {}.", { op, val1, val2 });
    }
    
    void var_redeclare(std::string var_name, std::string var_type) {
        std::cerr << style::style["red"] << style::style["bold"] << "Variable:" << style::style["reset"] << style::style["red"] << " Variable `" << var_name << "` of type `" << var_type << "` already exists." << style::style["reset"] << std::endl;
        exit(1);
    }

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
        //std::cerr << style::style["red"] << style::style["bold"] << "File: " << style::style["reset"] << style::style["red"] << " File `" << file_name << "` does not exist." << style::style["reset"] << std::endl;
        std::string line = "-";
        throw_err(globals, "File Not Found", "File {} not found.", { file_name });
    }

    void unidentified_keyword(Globals& globals, std::string& keyword) {
        //std::cerr << style::style["red"] << style::style["bold"] << "Kal:" << style::style["reset"] << style::style["red"] << " Keyword `" << keyword << "` is unidentified." << style::style["reset"] << std::endl;
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
}