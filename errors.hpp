#pragma once

#include <sstream>
#include <vector>

#include "lib/lib_style.hpp"

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
    void throw_err(std::string& line, std::string head, std::string body, std::initializer_list<std::string> list = {}, bool quit = true) {
        std::vector<std::string> args(list);
        for(std::string& each : args) {
            each = style::style["bold"] + style::style["yellow"] + each + style::style["reset"];
        }
        std::cerr << "\n\u250C\u2500\n\u2502 " << style::style["red"] << style::style["bold"] << style::style["underline"] << head << " Error" << style::style["reset"] << "\n\u2502 "
            << style::style["green"] << style::style["bold"] << "Line: " << write_line(line) << style::style["reset"] << "\n\u2502 "
            << fmt(body, args) << "\n\u2514\u2500\n" << std::endl; 
        if(quit) exit(1);
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
    
    void var_redeclare_error(std::string var_name, std::string var_type) {
        std::cerr << style::style["red"] << style::style["bold"] << "Variable:" << style::style["reset"] << style::style["red"] << " Variable `" << var_name << "` of type `" << var_type << "` already exists." << style::style["reset"] << std::endl;
        exit(1);
    }

    void change_const_var_error(std::string var_name) {
        std::cerr << style::style["red"] << style::style["bold"] << "Variable:" << style::style["reset"] << style::style["red"] << " Cannot modify `const` variable `" << var_name << "`." << std::endl;
        exit(1);
    }

    void types_incompatible_error(const std::string& var_name, const std::string& var_type, const std::string& second_var_name, const std::string& second_var_type) {
        std::cerr << style::style["red"] << style::style["bold"] << "Variable:" << style::style["reset"] << style::style["red"] << " Cannot assign variable `" << second_var_name << "` of type `" << second_var_type << "` to variable `" << var_name << "` of type `" << var_type << "`." << style::style["reset"] << std::endl;
        exit(1);
    }

    void unknown_var_type(const std::string& var_name, const std::string& var_type) {
        std::cerr << style::style["red"] << style::style["bold"] << "Variable:" << style::style["reset"] << style::style["red"] << " Type `" << var_type << "` of variable `" << var_name << "` is unknown." << style::style["reset"] << std::endl;
        exit(1);
    }

    void undefined_var_error(const std::string& var_name) {
        std::cerr << "\n" << style::style["red"] << style::style["bold"] << "Variable:" << style::style["reset"] << style::style["red"] << " Variable `" << var_name << "` is undefined." << style::style["reset"] << std::endl;
        exit(1);
    }

    void expected_type_error(std::string var_type) {
        std::cerr << style::style["red"] << style::style["bold"] << "Type: " << style::style["reset"] << style::style["red"] << "Data of type `" << var_type << "` expected." << std::endl;
        exit(1);
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

    void file_does_not_exist_error(const std::string& file_name) {
        std::cerr << style::style["red"] << style::style["bold"] << "File: " << style::style["reset"] << style::style["red"] << " File `" << file_name << "` does not exist." << style::style["reset"] << std::endl;
        exit(1);
    }

    void unidentified_keyword(const std::string& keyword) {
        std::cerr << style::style["red"] << style::style["bold"] << "Kal:" << style::style["reset"] << style::style["red"] << " Keyword `" << keyword << "` is unidentified." << style::style["reset"] << std::endl;
        exit(1);
    }
}
