#pragma once

#include <cstdlib>

#include "errors.hpp"
#include "lib/lib_path.hpp"
#include "lib/lib_string.hpp"

namespace preproc {
    void squash_vector(std::vector<std::string>& orignal, const std::vector<std::string>& other, int index, bool replace = true) {
        int orignal_size = orignal.size();
        for(int orignal_count = 0; orignal_count < orignal_size; orignal_count++) {
            if(orignal_count == index) {
                std::vector<std::string>::iterator orignal_itr = orignal.begin();
                if(replace) {
                    orignal[orignal_count] = other[0];
                }

                int start = 0;
                if(replace) {
                    start = 1;
                }

                int other_size = other.size();
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
            if(line[char_index] == ';' && !inside_string) {
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

    std::vector<std::string> clean_contents(std::vector<std::string>& line_contents) {
        std::vector<std::string> cleaned_contents;
        int line_contents_size = line_contents.size();

        for(int line_itr = 0; line_itr < line_contents_size; line_itr++) {
            line_contents[line_itr] = lib::trim_leading(line_contents[line_itr]);
            line_contents[line_itr] = lib::trim_trailing(line_contents[line_itr]);

            if(line_contents[line_itr] != "") {
                cleaned_contents.emplace_back(line_contents[line_itr]);
            }
        }

        return cleaned_contents;
    }

    std::vector<std::string> initial_preprocessing(std::string initial_file_path) {
        std::string initial_file_contents = lib::read_file(initial_file_path, true);
        remove_comments(initial_file_contents);
        adjust_strings(initial_file_contents);
        std::vector<std::string> initial_file_lines = lib::new_split(initial_file_contents);
        std::vector<std::string> initial_cleaned_file_lines = clean_contents(initial_file_lines);
        return initial_cleaned_file_lines;
    }

    void expand_files(std::vector<std::string>& expanded_contents, std::vector<std::string>& included_file_list, std::string& current_file_path) {
        char buf[4096];
        int clean_contents_size = expanded_contents.size();
        for(int content_itr = 0; content_itr < clean_contents_size; content_itr++) {
            if(expanded_contents[content_itr][0] == '@') {
                std::string include_file_path = expanded_contents[content_itr].substr(1);
                int include_file_path_size = include_file_path.size();
                if(include_file_path_size < 4 || include_file_path.substr(include_file_path_size - 4) != ".kal") {
                    include_file_path += ".kal";
                }
                include_file_path = /*include_file_path + '/' +*/ lib::get_dir(current_file_path) + include_file_path;
                const char* x = realpath(include_file_path.c_str(), buf);
                //std::cout << x << std::endl;
                include_file_path = x;
                std::cout << include_file_path << std::endl;
                if(lib::exists_in_vector(included_file_list, include_file_path)) {
                    errors::file_already_included_error(include_file_path);
                }
                if(include_file_path == current_file_path) {
                    errors::file_included_in_itself_error(include_file_path);
                }
                current_file_path = include_file_path;
                included_file_list.emplace_back(include_file_path);

                std::vector<std::string> included_cleaned_source_lines = initial_preprocessing(include_file_path);
                squash_vector(expanded_contents, included_cleaned_source_lines, content_itr);
                memset(buf, '\0', sizeof(buf));
            }
        }
    }

    void expand_deps(std::vector<std::string>& expanded_contents, std::string deps_str) {
        std::vector<std::string> deps;
        int size = deps_str.size();
        int index = size;
        int begin = size;

        std::string required_dep = "";
        /*if(deps_str[size - 1] != ',') {
            deps_str += ',';
            size++;
        }*/
        if(deps_str[0] != ',') {
            deps_str = ',' + deps_str;
            size++;
        }

        while(index >= 0) {
            //std::cout << index << ": " << deps_str[index] << std::endl;
            if(deps_str[index] == ',') {
                required_dep = deps_str.substr(index + 1, begin - index + 1);
                if(required_dep != "") {
                    required_dep = lib::trim_leading(lib::trim_trailing(required_dep));
                    deps.emplace_back(required_dep);
                }
                //std::cout << "[" << required_dep << "]" << std::endl;
                begin = index - 2;
            }
            index--;
        }
        //std::cout << deps_str << std::endl;
        /*while(index < size) {
            if(deps_str[index] == ',') {
                //std::cout << begin << " " << index << std::endl;
                required_dep = deps_str.substr(begin, index - begin);
                deps.emplace_back(required_dep);
                //std::cout << "[" << required_dep << "]" << std::endl;
                begin = index + 1;
            }
            index++;
        }*/
        //return;
        //std::string file_path = lib::get_dir(current_file_path) + file_path;
        for(std::string dep : deps) {
            std::vector<std::string> sloc = initial_preprocessing(dep);
            squash_vector(expanded_contents, sloc, 0, false);
        }

    }

    void preprocess(std::vector<std::string>& processed_contents, std::string& current_file_path/*, bool deps, std::string deps_str*/) {
        std::vector<std::string> included_file_list;
        /*if(deps) {
            expand_deps(processed_contents, deps_str, current_file_path)
        }*/
        for(uint64_t line_count = 0; line_count < processed_contents.size(); line_count++) {
            expand_files(processed_contents, included_file_list, current_file_path);
        }
    }
}
