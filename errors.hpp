#pragma once

#include "style.hpp"

namespace errors {
    void throw_err(std::string error_head, std::string error_body) {
        std::cerr << style::red << style::bold << error_head << ":" << style::reset << style::red << " " << error_body << style::reset << std::endl;
        exit(1);
    }

    void kal_error(std::string kal_err) {
        std::cerr << style::red << style::bold << "Kal:" << style::reset << style::red << " " << kal_err << style::reset << std::endl;
    }

    void stdout_error(std::string second_parameter) {
        std::cerr << style::red << style::bold << "stdout:" << style::reset << style::red << " stdout takes only one parameter but multiple parameters were passed: " << second_parameter << "..." << style::reset << std::endl;
    }
    
    void var_redeclare_error(std::string var_name, std::string var_type) {
        std::cerr << style::red << style::bold << "Variable:" << style::reset << style::red << " Variable `" << var_name << "` of type `" << var_type << "` already exists." << style::reset << std::endl;
    }
}
