#include <iostream>
#include <cstdlib>

//#include "shell.hpp"
#include "exec.hpp"
#include "kast.hpp"
#include "errors.hpp"
//#include "variable.hpp"
#include "arg_parser.hpp"
#include "preprocessor.hpp"
#include "lib/lib_path.hpp"
#include "lib/lib_string.hpp"

int main(int argc, char** argv) {
    ArgParser arg_parser = ArgParser(argc, argv);
    //VarTable var = VarTable();

    if(argc == 1) {
        //shell::init_shell();
        std::cout << "shell" << std::endl;
        return 0;
    }
    if(arg_parser.args_size() == 0) {
        errors::kal_error("Invalid argument passed.");
        return 1;
    }

    std::vector<std::string> args = arg_parser.get_args();
    std::string file_name;
    std::vector<std::string> source_lines;
    if(arg_parser.flag_exists("-p")) {
        file_name = arg_parser.get_value("-p");
        source_lines = preproc::preprocess(file_name);

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

    if(arg_parser.flag_exists("-k")) {
        std::string kast_file;
        std::string kal_file = arg_parser.get_value("-k");
        source_lines = preproc::preprocess(kal_file);
        std::vector<Token> tokens = lexer::tokenize(source_lines);
        if(arg_parser.flag_exists("-o")) {
            kast_file = arg_parser.get_value("-o");
            lib::ensure_extension(kast_file, ".kast");
        }
        else {
            kast_file = lib::replace_extension(kal_file, ".kast");
        }
        kast::encode(kast_file, tokens);

        return 0;
    }

    file_name = args[1];

    bool is_kast_file = file_name.substr(file_name.size() - 5) == ".kast";
    if(is_kast_file) {
        std::vector<Token> tokens;
        kast::decode(file_name, tokens);
        line_exec(tokens);
        return 0;
    }

    source_lines = preproc::preprocess(file_name);


    if(arg_parser.flag_exists("-d")) {
        std::string deps = arg_parser.get_value("-d");
        preproc::expand_deps(source_lines, deps);
    }

    int source_lines_count = source_lines.size();
    for(int each_line = 0; each_line < source_lines_count; each_line++) {
        lib::expand_tabs(source_lines[each_line]);
    }

    
    std::vector<Token> tokens = lexer::tokenize(source_lines);
    line_exec(tokens);

    VarTable::gc();

    return 0;
}
