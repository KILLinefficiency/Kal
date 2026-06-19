#pragma once

#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include "../errors.hpp"

namespace lib {
    std::string render_escape_chars(std::string text) {
        std::string to_replace;
        uint64_t index = text.find("\\", 0);
        while(index != std::string::npos) {
            if(text[index + 1] == 'n') {
                to_replace = "\n";
            }
            else if(text[index + 1] == 't') {
                to_replace = "\t";
            }
            else if(text[index + 1] == 'b') {
                to_replace = "\b";
            }
            else if(text[index + 1] == '\\') {
                to_replace = "\\";
            }
            else if(text[index + 1] == '"') {
                to_replace = "\"";
            }
            else if(text[index + 1] == 'r') {
                to_replace = "\r";
            }

            text.replace(index, 2, to_replace);
            index++;
            index = text.find("\\", index);
        }
        return text;
    }

    bool is_string(const std::string& text) {
        return text[0] == '"' && text[text.size() - 1] == '"';
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
            errors::file_does_not_exist_error(globals, file_path);
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
            if((line[line_index] == ' ') || (line[line_index] == '\t') || (line[line_index] == '\n')) {
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
            if((line[line_index] == ' ') || (line[line_index] == '\t') || (line[line_index] == '\n')) {
                white_space_len++;
                continue;
            }
            break;
        }
        
        int initial_str_len = size - white_space_len;

        std::string required_line = line.substr(0, initial_str_len);
        return required_line;
    }

    std::string trim(std::string& line) {
        const std::string half_trimmed = trim_leading(line);
        return trim_trailing(half_trimmed);
    }

    std::string vector_to_string(const std::vector<std::string>& text_vector, std::string join_text) {
        int line_size = 0;
        std::stringstream completed_text;
        for(const std::string& line : text_vector) {
            line_size = line.size();
            completed_text << line;
            if(line[line_size - 1] != '{' && line[line_size] != '}') {
                completed_text << '.';
            }
            completed_text << join_text;
        }
        return completed_text.str();
    }

    bool exists_in_vector(const std::vector<std::string>& text_list, const std::string& text) {
        for(std::string item : text_list) {
            if(item == text) {
                return true;
            }
        }
        return false;
    }

    std::string resolve_string(std::string text) {
        int text_size = text.size();
        std::string resolved_string = text;
        if(text[0] == '"' && text[text_size - 1] == '"') {
            resolved_string = text.substr(1, text_size - 2);
        }

        return resolved_string;
    }
    std::string resolve_string(std::string text, Globals& globals) {
        std::string resolved_string = text;

        int text_size = text.size();
        if(text[0] != '"' || text[text_size - 1] != '"') {
            Value* value = VarTable::get(text, {}, true, true, true, globals);
            if(value != nullptr) {
                resolved_string = (dynamic_cast<String*>(value))->str;
                text_size = resolved_string.size();
            }
            else {
                errors::undefined_var(globals, text);
            }
        }

        resolved_string = resolved_string.substr(1, text_size - 2);
        return resolved_string;
    }

    std::vector<std::string> split(std::string& text, char delimiter = '.', char secondary_id = '@', char secondary_delimiter = '\n', char escape_char = '"') {
        int index = 0;
        int text_size = text.size();
        int begin = 0;
        int end = 0;
        bool inside_string = false;
        std::string required_line;
        std::vector<std::string> lines;

        if(text[text_size - 1] != delimiter) {
            text += delimiter;
            text_size++;
        }

        while(index < text_size) {
            if(!inside_string && text[index] == '#' && text[index + 1] == '!') {
                while(text[index] != '\n') {
                    index++;
                }
                begin = ++index;
            }

            if(!inside_string && text[index] == secondary_id) {
                while(text[index] != secondary_delimiter) {
                    index++;
                }
                required_line = text.substr(begin, index - begin);
                lines.emplace_back(required_line);
                begin = ++index;
                continue;

            }

            if(!inside_string && (text.substr(index, 2) == "if" || text.substr(index, 4) == "else" || text.substr(index, 4) == "elif" || text.substr(index, 4) == "loop" || text.substr(index, 2) == "fn")) {
                index++;
                while(index < text_size && text[index - 1] != '{') {
                    index++;
                }
                if(index >= text_size) {
                    errors::closing_scope(text);
                }
                required_line = text.substr(begin, index - begin);
                lines.emplace_back(required_line);
                begin = index++;
                continue;
            }

            if(!inside_string && text[index] == '{') {
                lines.emplace_back("{");
                index++;
                while(text[index] == ' ' || text[index] == '\t' || text[index] == '\n' || text[index] == '\r') {
                    index++;
                }
                begin = index;
                continue;
            }

            if(!inside_string && text[index] == '}') {
                int end = index;
                required_line = text.substr(index, end - index + 1);
                lines.emplace_back(required_line);
                begin = ++index;
                continue;
            }

            if(text[index] == '.' && delimiter == '.') {
                if((text[index - 1] >= '0' && text[index - 1] <= '9') && (text[index + 1] >= '0' && text[index + 1] <= '9')) {
                    index++;
                    continue;
                }
            }

            if(((text_size - index) >= 3) && text[index] == '.' && delimiter == '.') {
                if(text[index + 1] == '.' && text[index + 2] == '.') {
                    index += 3;
                }
            }

            if((text_size - index >= 2) && text[index] == '.' && delimiter == '.') {
                if(text[index + 1] == '.') {
                    index += 2;
                }
            }

            if(!inside_string && text[index] == delimiter) {
                end = index;
                required_line = text.substr(begin, end - begin);
                if(required_line != "") {
                    lines.emplace_back(required_line);
                }
                begin = end + 1;
            }

            if(text[index] == escape_char && text[index - 1] != '\\') {
                inside_string = !inside_string;
            }
            if(inside_string && (index == (text_size - 1))) {
                // ERR:
                errors::string_eol(text);
            }
            index++;
        }

        return lines;
    }
}
