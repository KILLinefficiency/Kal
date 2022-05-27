#include <iostream>
#include "arg_parser.hpp"
#include "lib/lib_string.hpp"
#include "preprocessor/remove_comments.hpp"
#include "errors.hpp"
#include "preprocessor/trim_spaces.hpp"
#include "preprocessor/expand_files.hpp"

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
    std::string source = lib::read_file(file_name);
    std::cout << source << std::endl;
    return 0;
}
