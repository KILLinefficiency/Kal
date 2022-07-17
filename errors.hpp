#pragma once

#include "lib/lib_style.hpp"

namespace errors {
    void throw_err(std::string error_head, std::string error_body) {
        std::cerr << style::red << style::bold << error_head << ":" << style::reset << style::red << " " << error_body << style::reset << std::endl;
        exit(1);
    }

    void kal_error(std::string kal_err) {
        std::cerr << style::red << style::bold << "Kal:" << style::reset << style::red << " " << kal_err << style::reset << std::endl;
        exit(1);
    }
    
    void var_redeclare_error(std::string var_name, std::string var_type) {
        std::cerr << style::red << style::bold << "Variable:" << style::reset << style::red << " Variable `" << var_name << "` of type `" << var_type << "` already exists." << style::reset << std::endl;
        exit(1);
    }

    void change_const_var_error(std::string var_name) {
        std::cerr << style::red << style::bold << "Variable:" << style::reset << style::red << " Cannot modify `const` variable `" << var_name << "`." << std::endl;
        exit(1);
    }

    void types_incompatible_error(const std::string& var_name, const std::string& var_type, const std::string& second_var_name, const std::string& second_var_type) {
        std::cerr << style::red << style::bold << "Variable:" << style::reset << style::red << " Cannot assign variable `" << second_var_name << "` of type `" << second_var_type << "` to variable `" << var_name << "` of type `" << var_type << "`." << style::reset << std::endl;
        exit(1);
    }

    void unknown_var_type(const std::string& var_name, const std::string& var_type) {
        std::cerr << style::red << style::bold << "Variable:" << style::reset << style::red << " Type `" << var_type << "` of variable `" << var_name << "` is unknown." << style::reset << std::endl;
        exit(1);
    }

    void undefined_var_error(const std::string& var_name) {
        std::cerr << "\n" << style::red << style::bold << "Variable:" << style::reset << style::red << " Variable `" << var_name << "` is undefined." << style::reset << std::endl;
        exit(1);
    }

    void file_already_included_error(const std::string& file_name) {
        std::cerr << style::red << style::bold << "Preprocessor:" << style::reset << style::red << " File `" << file_name << "` is already included." << style::reset << std::endl;
        exit(1);
    }

    void file_included_in_itself_error(const std::string& file_name) {
        std::cerr << style::red << style::bold << "Preprocessor:" << style::reset << style::red << " Cannot include file `" << file_name << "` in itself." << style::reset << std::endl;
        exit(1);
    }

    void unidentified_keyword(const std::string& keyword) {
        std::cerr << style::red << style::bold << "Kal:" << style::reset << style::red << " Keyword `" << keyword << "` is unidentified." << style::reset << std::endl;
        exit(1);
    }
}
