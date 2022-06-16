#pragma once

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

    std::vector<std::string> clean_contents(std::vector<std::string>& line_contents) {
        //std::vector<std::string> line_contents = lib::split(file_contents, '\n');
        std::vector<std::string> cleaned_contents;
        int line_contents_size = line_contents.size();

        for(int line_itr = 0; line_itr < line_contents_size; line_itr++) {
            line_contents[line_itr] = remove_comments(line_contents[line_itr]);
            line_contents[line_itr] = lib::trim_leading(line_contents[line_itr]);
            line_contents[line_itr] = lib::trim_trailing(line_contents[line_itr]);

            if(line_contents[line_itr] != "") {
                cleaned_contents.emplace_back(line_contents[line_itr]);
            }
        }

        return cleaned_contents;
    }

    std::vector<std::string> initial_preprocessing(std::string initial_file_path) {
        std::string initial_file_contents = lib::read_file(initial_file_path);
        std::vector<std::string> initial_file_lines = lib::split(initial_file_contents, '\n');
        std::vector<std::string> initial_cleaned_file_lines = clean_contents(initial_file_lines);
        return initial_cleaned_file_lines;
    }

    void expand_files(std::vector<std::string>& expanded_contents) {
        int clean_contents_size = expanded_contents.size();
        for(int content_itr = 0; content_itr < clean_contents_size; content_itr++) {
            if(expanded_contents[content_itr][0] == '@') {
                int file_path_size = expanded_contents[content_itr].size();
                std::string include_file_path = expanded_contents[content_itr].substr(1, file_path_size - 1);
                std::string included_source_contents = lib::read_file(include_file_path);
                std::vector<std::string> included_source_lines = lib::split(included_source_contents, '\n');
                std::vector<std::string> included_cleaned_source_lines = clean_contents(included_source_lines);
                squash_vector(expanded_contents, included_cleaned_source_lines, content_itr);
            }
        }
    }
}
