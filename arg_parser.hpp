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
        //int skip = 0;
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

            /*std::unordered_map<std::string, std::string>::iterator it;
            for(it = flags_map.begin(); it != flags_map.end(); it++) {
                std::cout << it->first << ": " << it->second << std::endl;
            }
            for(std::string x : prog_args) {
                std::cout << x << std::endl;
            }*/
            //std::cout << "--------\n";
            /*for(int item_itr = 0; item_itr < size; item_itr++) {
               if(args[item_itr][0] == '-') {
                   int next_item = item_itr + 1;
                   if(next_item == size) {
                        break;
                   }
                   flags_map[args[item_itr]] = args[next_item];
                   //skip = next_item + 1;
               }
               //std::cout << args[skip] << std::endl;
            }
            
            int arg_start_point = 0;

            for(int arg_itr = 1; arg_itr < size; arg_itr++) {
                std::string arg_str = std::string(args[arg_itr]);
                if(lib::ends_with(arg_str, file_extension)) {
                    arg_start_point = arg_itr;
                    break;
                }
                else if(arg_str[0] != '-') {
                    errors::kal_error("File `" + arg_str + "` is an invalid Kal file.");
                }
            }

            if(arg_start_point == 0) {
                prog_args = {};
                arg_start_point = 1;
            }

            for(int start = arg_start_point; start < size; start++) {
                prog_args.emplace_back(std::string(args[start]));
            }*/

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
