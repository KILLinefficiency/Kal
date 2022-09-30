#pragma once

#include "errors.hpp"
#include "lib/lib_string.hpp"

namespace preproc {
    void squash_vector(std::vector<std::string>& orignal, const std::vector<std::string>& other, int index) {
        int orignal_size = orignal.size();
        for(int orignal_count = 0; orignal_count < orignal_size; orignal_count++) {
            if(orignal_count == index) {
                std::vector<std::string>::iterator orignal_itr = orignal.begin();
                orignal[orignal_count] = other[0];

                int other_size = other.size();
                for(int other_count = 1; other_count < other_size; other_count++) {
                    orignal.insert(orignal_itr + orignal_count + other_count, other[other_count]);
                    orignal_itr = orignal.begin();
                }
                break;
            }
        }
    }

    std::string remove_comments(const std::string& line) {
        if(line == "" || line == ";" || line[0] == ';') {
            return "";
        }

        bool enable_removal = true;
        int required_len = 0;
        int line_size = line.size();
        for(int char_index = 0; char_index < line_size; char_index++) {
            if(line[char_index] == '"') {
                enable_removal = !enable_removal;
            }
            if((line[char_index] == ';') && enable_removal) {
                break;
            }
            required_len++;
        }
        
        std::string useful_string = line.substr(0, required_len);
        return useful_string;
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
            line_contents[line_itr] = remove_comments(line_contents[line_itr]);
            line_contents[line_itr] = lib::trim_leading(line_contents[line_itr]);
            line_contents[line_itr] = lib::trim_trailing(line_contents[line_itr]);
            adjust_strings(line_contents[line_itr]);

            if(line_contents[line_itr] != "") {
                cleaned_contents.emplace_back(line_contents[line_itr]);
            }
        }

        return cleaned_contents;
    }

    std::vector<std::string> initial_preprocessing(std::string initial_file_path) {
        std::string initial_file_contents = lib::read_file(initial_file_path, true);
        std::vector<std::string> initial_file_lines = lib::split(initial_file_contents, '\n');
        std::vector<std::string> initial_cleaned_file_lines = clean_contents(initial_file_lines);
        return initial_cleaned_file_lines;
    }

    void expand_files(std::vector<std::string>& expanded_contents, std::vector<std::string>& included_file_list, std::string& current_file_path) {
        int clean_contents_size = expanded_contents.size();
        for(int content_itr = 0; content_itr < clean_contents_size; content_itr++) {
            if(expanded_contents[content_itr][0] == '@') {
                std::string include_file_path = expanded_contents[content_itr].substr(1);
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
            }
        }
    }

    void preprocess(std::vector<std::string>& processed_contents, std::string& current_file_path) {
        std::vector<std::string> included_file_list;
        for(uint64_t line_count = 0; line_count < processed_contents.size(); line_count++) {
            expand_files(processed_contents, included_file_list, current_file_path);
        }
    }
}
