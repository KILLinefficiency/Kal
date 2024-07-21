#pragma once

#include <cstdlib>
#include <vector>
#include <stack>

#include "errors.hpp"
#include "lib/lib_path.hpp"
#include "lib/lib_string.hpp"

namespace preproc {
    void squash_vector(std::vector<std::string>& orignal, const std::vector<std::string>& other, int index, bool replace = true) {
        int orignal_size = orignal.size();
        for(int orignal_count = 0; orignal_count < orignal_size; orignal_count++) {
            if(orignal_count == index) {
                if(replace) {
                    orignal[orignal_count] = other[0];
                }

                int start = 0;
                if(replace) {
                    start = 1;
                }

                int other_size = other.size();
                orignal.reserve(orignal.capacity() + (replace ? (other_size - 1) : other_size));
                std::vector<std::string>::iterator orignal_itr = orignal.begin();
                for(int other_count = start; other_count < other_size; other_count++) {
                    orignal.insert(orignal_itr + orignal_count + other_count, other[other_count]);
                    orignal_itr = orignal.begin();
                }
                break;
            }
        }
    }

    void remove_comments(std::string& line) {
        bool inside_string = false;
        bool enable_removal = false;
        int line_size = line.size();
        int char_index = 0;
        while(char_index < line_size) {
            if(line[char_index] == '"') {
                inside_string = !inside_string;
            }
            if(line[char_index] == ';' && line[char_index + 1] == ';' && !inside_string) {
                while(char_index < line_size && line[char_index] != '\n') {
                    line[char_index] = ' ';
                    char_index++;
                }
            }
            else if(line[char_index] == ';' && !inside_string) {
                line[char_index] = ' ';
                enable_removal = !enable_removal;
            }
            if(enable_removal) {
                line[char_index] = ' ';
            }
            char_index++;
        }
    }

    void adjust_strings(std::string& line, char delimiter = ' ') {
        bool inside = false;
        const char str_delim[2] = { delimiter, '\0' };
        for(uint64_t line_itr = 0; line_itr < line.size(); line_itr++) {
            if(line[line_itr] == '"' && line[line_itr - 1] != delimiter && line_itr != 0 && !inside) {
                line.insert(line_itr, str_delim);
            }
            if(line[line_itr] == '"') {
                inside = !inside;
            }
        }

        inside = false;
        for(uint64_t line_itr = 0; line_itr < line.size(); line_itr++) {
            if(line[line_itr] == '"') {
                inside = !inside;
            }
            if(line[line_itr] == '"' && line[line_itr + 1] != delimiter && line_itr != line.size() - 1 && !inside) {
                line.insert(line_itr + 1, str_delim);
            }
        }

        for(uint64_t line_itr = 0; line_itr < line.size(); line_itr++) {
            if(line[line_itr] == '$' && line[line_itr - 1] != delimiter && line[line_itr - 1] != '#' && line_itr != 0) {
                line.insert(line_itr, str_delim);
            }
        }
    }

    void check_path(std::vector<std::string>& paths, std::string path) {
        uint64_t size = paths.size();
        if(path == paths[size - 1]) {
            std::cerr << paths[size - 1] << " included in itself\n"; exit(1);
        }
        for(std::string each : paths) {
            if(each == path) {
                std::cerr << each << " already included\n"; exit(1);
            } 
        }
    }


    std::stack<std::string> dirs;
    std::vector<std::string> paths;
    std::vector<std::string> preprocess(std::string file_path) {
        std::vector<std::string> all_lines;
        std::string top = "";
        if(!dirs.empty()) {
            top = dirs.top();
        }
        std::string abs_file_path = lib::get_path(file_path, top);
        std::string current_path = lib::get_dir(abs_file_path);

        std::string file_contents = lib::read_file(abs_file_path);
        remove_comments(file_contents);

        std::vector<std::string> file_lines = lib::new_split(file_contents);
        paths.emplace_back(abs_file_path);
        for(std::string& line : file_lines) {
            line = lib::trim_leading(lib::trim_trailing(line));
            if(line != "") {
                all_lines.emplace_back(line);
            }

            if(line[0] == '@') {
                std::string include_path = line.substr(1);
                lib::ensure_extension(include_path, ".kal");
                dirs.push(current_path);
                abs_file_path = include_path;
                check_path(paths, lib::get_path(abs_file_path));
                std::vector<std::string> vals = preprocess(abs_file_path);
                squash_vector(all_lines, vals, all_lines.size() - 1);
                current_path = dirs.top();
                dirs.pop();
            }
        }

        return all_lines;
    }


    void expand_deps(std::vector<std::string>& expanded_contents, std::string deps_str) {
        std::vector<std::string> deps;
        int size = deps_str.size();
        int index = size;
        int begin = size;

        std::string required_dep = "";
        if(deps_str[0] != ',') {
            deps_str = ',' + deps_str;
            size++;
        }

        while(index >= 0) {
            if(deps_str[index] == ',') {
                required_dep = deps_str.substr(index + 1, begin - index + 1);
                if(required_dep != "") {
                    required_dep = lib::trim_leading(lib::trim_trailing(required_dep));
                    deps.emplace_back(required_dep);
                }
                begin = index - 2;
            }
            index--;
        }
        for(std::string dep : deps) {
            std::vector<std::string> sloc = preprocess(dep);
            squash_vector(expanded_contents, sloc, 0, false);
        }

    }

}
