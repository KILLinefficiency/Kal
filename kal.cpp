#include <iostream>
#include "errors.hpp"
#include "arg_parser.hpp"
#include "preprocessor.hpp"
#include "lib/lib_string.hpp"

int main(int argc, char** argv) {
    ArgParser arg_parser = ArgParser(argc, argv);
    if(argc == 1) {
        errors::kal_error("No arguments passed.");
        return 1;
    }
    if(arg_parser.args_size() == 0) {
        errors::kal_error("Invalid argument passed.");
        return 1;
    }

    std::string file_name = arg_parser.get_arg(0);
    std::vector<std::string> source_lines = preproc::initial_preprocessing(file_name);
    preproc::preprocess(source_lines);

    for(std::string line : source_lines) {
        std::cout << "[" << line << "]" << std::endl;
    }

    return 0;
}
