#include <iostream>

#include "shell.hpp"
#include "parser.hpp"
#include "errors.hpp"
#include "variable.hpp"
#include "arg_parser.hpp"
#include "preprocessor.hpp"
#include "lib/lib_string.hpp"

int main(int argc, char** argv) {
    ArgParser arg_parser = ArgParser(argc, argv);
    VarTable var = VarTable();

    if(argc == 1) {
        shell::init_shell();
        return 0;
    }
    if(arg_parser.args_size() == 0) {
        errors::kal_error("Invalid argument passed.");
        return 1;
    }

    std::string file_name = arg_parser.get_arg(0);
    std::vector<std::string> source_lines = preproc::initial_preprocessing(file_name);
    preproc::preprocess(source_lines, file_name);

    int source_lines_count = source_lines.size();
    for(int each_line = 0; each_line < source_lines_count; each_line++) {
        lib::expand_tabs(source_lines[each_line]);
    }

    if(arg_parser.flag_exists("-p")) {

        if(arg_parser.flag_exists("-o")) {
            std::string write_path = arg_parser.get_value("-o");
            std::string preprocessed_code = lib::vector_to_string(source_lines, "\n");
            lib::write_file(write_path, preprocessed_code);
            return 0;
        }

        int source_lines_size = source_lines.size();
        for(int each_line = 0; each_line < source_lines_size; each_line++) {
            std::cout << source_lines[each_line] << '\n';
        }
        return 0;
    }
    
    std::vector<std::vector<std::string>> tokens = lexer::tokenize(source_lines);
    line_exec(tokens, var);

    return 0;
}
