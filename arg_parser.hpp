#pragma once

#include <vector>
#include <unordered_map>
#include "lib/lib_string.hpp"

class ArgParser {
    private:
        std::string file_extension = ".kal";
        std::vector<std::string> prog_args;
        std::unordered_map<std::string, std::string> flags_map;

    public:
        ArgParser(const int& size, char** args) {
            int index = 0;
            while(index < size) {
                if(args[index][0] == '-') {
                    int next_item = index + 1;
                    if(next_item == size) {
                        break;
                    }
                    flags_map[args[index]] = args[next_item];
                    index++;
                }
                else {
                    prog_args.emplace_back(args[index]);
                }
                index++;
            }
        }

        std::vector<std::string> get_args() {
            return prog_args; 
        }

        int args_size() {
            int total_args = prog_args.size();
            return total_args;
        }

        std::string get_value(std::string flag) {
            std::string value = flags_map[flag];
            return value;
        }

        bool flag_exists(std::string key) {
            bool exists = flags_map.find(key) != flags_map.end();
            return exists;
        } 
};
