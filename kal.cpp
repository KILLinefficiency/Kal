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
    
    if(arg_parser.flag_exists("-p")) {
        std::string preprocessed_code = lib::vector_to_string(source_lines);

        if(arg_parser.flag_exists("-o")) {
            std::string write_path = arg_parser.get_value("-o");
            lib::write_file(write_path, preprocessed_code);
            return 0;
        }

        std::cout << preprocessed_code << std::endl;
        return 0;
    }

    return 0;
}
