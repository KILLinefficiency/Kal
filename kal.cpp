#include <iostream>
#include "arg_parser.hpp"
#include "lib/lib_string.hpp"
#include "errors.hpp"
#include "preprocessor.hpp"

int main(int argc, char** argv) {
    if(argc == 1) {
        errors::kal_error("No arguments passed.");
        return 1;
    }
    std::vector<std::string> args = get_args(argv, argc);
    if(args.size() == 0) {
        errors::kal_error("Invalid argument passed.");
        return 1;
    }
    std::unordered_map<std::string, std::string> flags = get_flags(argv, argc);
    
    std::string& file_name = args[0];
    std::vector<std::string> initial_read = preproc::initial_preprocessing(file_name);

    preproc::expand_files(initial_read);
    for(std::string line : initial_read) {
        std::cout << "[" << line << "]" << std::endl;
    }

    return 0;
}
