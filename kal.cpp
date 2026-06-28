#include <iostream>
#include <cstdlib>

#include "globals.hpp"
#include "shell.hpp"
#include "exec.hpp"
#include "kast.hpp"
#include "errors.hpp"
#include "var.hpp"
#include "arg_parser.hpp"
#include "preprocessor.hpp"
#include "lib/lib_path.hpp"
#include "lib/lib_string.hpp"

int main(int argc, char** argv) {
    std::srand(std::time(0));
    globals.clock_start = std::chrono::high_resolution_clock::now();

    Memory& memory = globals.memory;
    ArgParser arg_parser = ArgParser(argc, argv);
    int arg_size = arg_parser.args_size();

    if(argc == 1) {
        shell::init_shell();
        return 0;
    }
    if(arg_size == 0) {
        errors::kal_error("Invalid argument passed.");
        return 1;
    }

    std::vector<std::string> args = arg_parser.get_args();
    std::string file_name;
    std::vector<std::string> source_lines;
    if(arg_parser.flag_exists("-p")) {
        file_name = arg_parser.get_value("-p");
        source_lines = preproc::preprocess_file(file_name);
        std::string preprocessed_code = lib::vector_to_string(source_lines, "\n");

        if(arg_parser.flag_exists("-o")) {
            std::string write_path = arg_parser.get_value("-o");
            lib::write_file(write_path, preprocessed_code);
            return 0;
        }

        std::cout << preprocessed_code << "\n";
        return 0;
    }

    if(arg_parser.flag_exists("-k")) {
        std::string kast_file;
        std::string kal_file = arg_parser.get_value("-k");
        source_lines = preproc::preprocess_file(kal_file);
        std::vector<Token> tokens = lexer::tokenize(source_lines);
        if(arg_parser.flag_exists("-o")) {
            kast_file = arg_parser.get_value("-o");
            lib::ensure_extension(kast_file, ".kast");
        }
        else {
            kast_file = lib::replace_extension(kal_file, ".kast");
        }
        kast::encode(kast_file, tokens, Functions::fn);

        return 0;
    }

    if(arg_size == 1) {
        // ERR:
        std::string line = "-";
        errors::no_cmd_arg(line);
    }

    file_name = args[1];

    memory["ARGS"] = new List();
    (dynamic_cast<List*>(memory["ARGS"]))->items.reserve(args.size());
    for(std::string& arg : args) {
        (dynamic_cast<List*>(memory["ARGS"]))->items.emplace_back(new String('"' + arg + '"'));
    }

    if(arg_parser.flag_exists("-x")) {
        std::string pkg_name = arg_parser.get_value("-x");
        std::vector<Value*>::iterator name_pos = (dynamic_cast<List*>(memory["ARGS"]))->items.begin() + 1;
        (dynamic_cast<List*>(memory["ARGS"]))->items.insert(name_pos, new String('"' + pkg_name + '"'));
        file_name = lib::resolve_package(pkg_name, "cli.kal");
    }

    bool is_kast_file = file_name.substr(file_name.size() - 5) == ".kast";
    if(is_kast_file) {
        std::vector<Token> tokens;
        kast::decode(file_name, tokens, Functions::fn);
        line_exec(tokens, false, true, false, globals);

        globals.depth = 0;
        VarTable::gc(globals);
        return 0;
    }

    source_lines = preproc::preprocess_file(file_name);

    if(arg_parser.flag_exists("-d")) {
        std::string deps = arg_parser.get_value("-d");
        preproc::expand_deps(source_lines, deps);
    }
    
    std::vector<Token> tokens = lexer::tokenize(source_lines);
    line_exec(tokens, false, true, false, globals);

    globals.depth = 0;
    VarTable::gc(globals);

    return 0;
}
