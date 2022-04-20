#include <vector>
#include <unordered_map>
#include "lib/lib_string.cpp"

std::unordered_map<std::string, std::string> get_flags(char** args, const int& size) {
    std::unordered_map<std::string, std::string> flags_map;
    for(int item_itr = 0; item_itr < size; item_itr++) {
       if(args[item_itr][0] == '-') {
           int next_item = item_itr + 1;
           if(next_item == size) {
                break;
           }
           flags_map[args[item_itr]] = args[next_item];
       }
    }

    return flags_map;
}

std::vector<std::string> get_args(char** args, const int& size) {
    int arg_start_point = 0;

    std::string file_extension = ".kal";
    std::vector<std::string> prog_args;

    for(int arg_itr = 1; arg_itr < size; arg_itr++) {
        std::string arg_str = std::string(args[arg_itr]);
        if(lib::ends_with(arg_str, file_extension)) {
            arg_start_point = arg_itr;
            break;
        }
    }

    if(arg_start_point == 0) {
        return std::vector<std::string> {};
    }

    for(int start = arg_start_point; start < size; start++) {
        prog_args.push_back(std::string(args[start]));
    }

    return prog_args;
}
