#pragma once

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../errors.hpp"

namespace lib {
    std::string render_escape_chars(std::string text) {
        int text_size = text.size();

        for(int current_char = 0; current_char < text_size; current_char++) {
            if(text[current_char] == '\\') {
                if(text[current_char + 1] == 'n') {
                    text[current_char] = '\n';
                    text[current_char + 1] = '\0';
                }
                else if(text[current_char + 1] == 't') {
                    text[current_char] = '\t';
                    text[current_char + 1] = '\0';
                }
            }
        }

        return text;
    }

    bool ends_with(const std::string& major_string, const std::string& minor_string) {
        int major_string_size = major_string.size();
        int minor_string_size = minor_string.size();

        if(major_string_size < minor_string_size) {
            return false;
        }

        int diff_size = major_string_size - minor_string_size;
        return major_string.substr(diff_size) == minor_string;
    }

    std::string read_file(std::string file_path, bool by_interpreter = false) {
        std::ifstream source_file(file_path);
        if(!source_file.good()) {
            if(by_interpreter) {
                errors::kal_error("File `" + file_path + "` does not exist.");
            }
            std::cout << "Kal: File " << file_path << " does not exist." << std::endl;
            return "";
        }
        std::stringstream file_contents;
        file_contents << source_file.rdbuf();
        source_file.close();
        return file_contents.str();
    }

    void write_file(std::string file_path, std::string& text) {
        std::ofstream destination_file(file_path);
        destination_file << text;
        destination_file.close();
    }

    std::string trim_leading(const std::string& line) {
        int size = line.size();
        int white_space_len = 0;
        for(int line_index = 0; line_index < size; line_index++) {
            if((line[line_index] == ' ') || (line[line_index] == '\t')) {
                white_space_len++;
                continue;
            }
            break;
        }

        int rest_str_len = size - white_space_len;

        std::string required_line = line.substr(white_space_len, rest_str_len);
        return required_line;
    }

    std::string trim_trailing(const std::string& line) {
        int size = line.size();
        int white_space_len = 0;
        for(int line_index = size - 1; line_index > 0; line_index--) {
            if((line[line_index] == ' ') || (line[line_index] == '\t')) {
                white_space_len++;
                continue;
            }
            break;
        }
        
        int initial_str_len = size - white_space_len;

        std::string required_line = line.substr(0, initial_str_len);
        return required_line;
    }

    std::string vector_to_string(const std::vector<std::string>& text_vector, std::string join_text = " ", int begin = 0) {
        std::string complete_text = "";
        int vector_size = text_vector.size();

        for(int each_line = begin; each_line < vector_size; each_line++) {
            complete_text += text_vector[each_line] + join_text;
        }

        return complete_text;
    }

    bool exists_in_vector(const std::vector<std::string>& text_list, const std::string& text) {
        for(std::string item : text_list) {
            if(item == text) {
                return true;
            }
        }
        return false;
    }

    std::vector<std::string> split(std::string& text, char delimiter = ' ', char escape_char = '"') {
        int len = -1;
        int begin = 0;
        int size = text.size();
        bool enable_split = true;
        std::vector<std::string> words;

        if(text[0] != delimiter) {
            text = delimiter + text;
            size++;
        }

        if(text[size - 1] != delimiter) {
            text += delimiter;
            size++;
        }

        for(int current_index = 0; current_index < size; current_index++) {
            if((text[current_index] == escape_char) && ((text[current_index - 1] == delimiter) || !enable_split)) {
                enable_split = !enable_split;
                if(enable_split) {
                    begin++;
                }
                else {
                    len -= 2;
                }
            }

            len++;

            if((text[current_index] == delimiter) && enable_split) {
                std::string required_string = text.substr(begin, len);
                if(required_string != "") {
                    words.emplace_back(required_string);
                }
                begin = current_index + 1;
                len = -1;
            }
        }

        return words;
    }

}
