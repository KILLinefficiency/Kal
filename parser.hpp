#pragma once

#define END std::cout << "syntax error\n"; exit(1)

#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>

#include "config.hpp"

#define WHITESPACE(position) (text[position] == ' ' || text[position] == '\t' || text[position] == '\n' || text[position] == '\r')

std::string eval(std::string expr);

namespace parser {
    // for now.
    std::string extract_list(const std::string&, int&);
    // for now.
    std::string general_delimiter = ",";
    std::string str_delimiter = "\"";
    std::string list_open = "[";
    std::string list_close = "]";
    std::string target_operator = "->";
    std::string assignment_operator = "=";
    std::string null_val = "null";

    std::string slice(const std::string& text, int start, int end) {
        int diff = end - start;
        std::string required_slice = text.substr(start, diff);
        return required_slice;
    }

    bool is_alpha(const char& character) {
        return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z');
    }

    bool is_num(const char& number) {
        return (number >= '0' && number <= '9') || number == '.';
    }

    bool is_string(const std::string& str_token) {
        return str_token[0] == '"' && str_token[str_token.size() - 1] == '"';
    }

    bool is_var(const std::string& var_token) {
        return var_token[0] == '$';
    }

    bool is_number(char number) {
        return number >= '0' && number <= '9';
    }

    bool match(int& index, std::string text, const std::string pattern, bool incr = true) {
        int pattern_len = pattern.size();
        std::string found_pattern = text.substr(index, pattern_len);
        bool matched = pattern == found_pattern;
        if(matched && incr) {
            index += pattern_len;
        }
        return matched;
    }

    std::string parse_number(const std::string& text, int& index) {
        int begin = index;
        while(is_number(text[index]) || text[index] == '.') {
            index++;
        }
        int end = index;
        index--;
        return text.substr(begin, end - begin);
    }

    std::string parse_string(const std::string& text, int& index) {
        int begin = index;
        index++;
        while(text[index] != '"') {
            index++;
        }
        int end = index;
        std::string required_string = text.substr(begin, end - begin + 1);
        return required_string;
    }

    std::string parse_expr(const std::string& text, int& index) {
        int begin = index + 1;
        index++;
        while(text[index] != '`') {
            index++;
        }
        int end = index - 1;
        std::string expr = text.substr(begin, end - begin + 1);
        return expr;
    }

    std::string parse_variable(const std::string& text, int& index) {
        int begin = index;
        int text_pos = index;
        int text_size = text.size();
        if(text[index] == '$') {
            index++;
            if(text[index] == '&') {
                index++;
            }
        }
        if(text[index] >= '0' && text[index] <= '9') {
            END;
        }
        while(text_pos < text_size) {
            if(is_alpha(text[index]) || text[index] == '_' || (text[index] >= '0' && text[index] <= '9')) {
                index++;
            }
            else {
                if(text[index] == '\0' || text[index] == '=' || text[index] == ',' || WHITESPACE(index) || text[index] == '[') {
                    break;
                }
                END;
            }
            text_pos++;
        }
        int end = index;
        //index--;
        std::string required_string = text.substr(begin, end - begin);
        if(text[index] == '[') {
            std::string sub_body = extract_list(text, index);
            sub_body = sub_body.substr(1, sub_body.size() - 2);
            sub_body = eval(sub_body);
            required_string += "[" + sub_body + "]";
        }
        return required_string;
    }

    std::string extract_list(const std::string& text, int& index) {
        int depth = 0;
        int start = index;
        while(text[index] != ']' || depth != 0) {
            if(text[index] == '[') {
                depth++;
            }
            if(text[index + 1] == ']') {
                depth--;
            }
            index++;
        }
        return text.substr(start, index - start + 1);
    }

    std::unordered_map<std::string, std::string> parse_list(std::string& text, int index = 0) {
        int pos = -1;
        int depth = 0;
        std::stack<int> indices;
        std::vector<int> prev_indices;
        std::string element;
        std::unordered_map<std::string, std::string> elements;
        bool inside_string = false;
        index++;
        while(WHITESPACE(index)) {
            index++;
        }
        int begin = index;
        int end = index;
        while(text[index] != ']' || depth != 0 || inside_string) {
            if(text[index] == '[' && !inside_string) {
                indices.push(pos + 1);
                prev_indices.push_back(pos + 1);
                pos = -1;
                begin++;
                index++;
                depth++;
                continue;
            }
            if(text[index] == ']' && !inside_string) {
                prev_indices.clear();
                pos = indices.top();
                indices.pop();
                begin++;
                index++;
                depth--;
                continue;
            }
            if(text[index] == '"') {
                inside_string = !inside_string;
            }
            if(inside_string) {
                index++;
                continue;
            }
            while(text[index] != ',') {
                if(text[index + 1] == ']') {
                    break;
                }
                index++;
            }
            end = index;
            if(text[index + 1] == ']' && text[index] != ',') {
                end++;
            }
            while(WHITESPACE(end - 1)) {
                end--;
            }
            element = text.substr(begin, end - begin);
            index++;
            while(WHITESPACE(index)) {
                index++;
            }
            begin = index;
            if(element != "") { 
                pos++;
                std::string element_idx = "#";
                for(int idx : prev_indices) {
                    element_idx += (std::to_string(idx) + "#");
                }
                element_idx += std::to_string(pos);
                elements[element_idx] = element;
                //std::cout << element_idx << ": " << element << std::endl;;
            }
        }

        return elements;
    }

    std::string parse_value(const std::string& text, int& index) {
        std::string required_token = "";

        if(is_num(text[index])) {
            required_token = parse_number(text, index);
        }
        else if(text[index] == '$') {
            required_token = parse_variable(text, index);
        }
        else if(text[index] == '[') {
            required_token = extract_list(text, index);
        }
        else if(text[index] == '`') {
            required_token = parse_expr(text, index);
        }
        else if(text[index] == '"') {
            required_token = parse_string(text, index);
        }
        else if(match(index, text, null_val)) {
            required_token = null_val;
            index--;
        }
        else if(text[index] == '\0') {
            return required_token;
        }
        else {
            std::cout << "unknown token: " << text[index] << std::endl;
            exit(1);
        }

        return required_token;
    }

    /*std::vector<std::string> parse_init(const std::string& text, int& index) {
        int begin = index;
        int end = index;
        int text_size = text.size();
        std::vector<std::string> tokens;
        std::string required_token = "";
        while(index < text_size) {
            bool key_val = false;
            if(text[index] == '=') {
                key_val = true;
                end = index;
                while(WHITESPACE(end - 1)) {
                    end--;
                }
                required_token = text.substr(begin, end - begin);
                tokens.emplace_back(required_token);

                index++;
                while(WHITESPACE(index)) {
                    index++;
                }
                required_token = parse_value(text, index);
                tokens.emplace_back(required_token);
                index++;
                while(WHITESPACE(index)) {
                    index++;
                }
            }
            if(text[index] == ',' || index == text_size - 1) {
                if(!key_val) {
                    int offset = 1;
                    if(index == text_size - 1 && text[text_size - 1] != ',') {
                        offset++;
                    }
                    int var_end = index;
                    while(WHITESPACE(var_end - 1)) {
                        var_end--;
                    }
                    var_end--;
                    int var_start = var_end;
                    while(is_alpha(text[var_start - 1])) {
                        var_start--;
                    }
                    required_token = text.substr(var_start, var_end - var_start + offset);
                    tokens.emplace_back(required_token);
                    tokens.emplace_back(null_val);
                }
                index++;
                while(WHITESPACE(index)) {
                    index++;
                }
                begin = index;
                continue;
            }
            if(match(index, text, target_operator, false)) {
                END;
            }
            index++;
        }

        return tokens;
    }*/

    std::vector<std::string> parse_init(const std::string& text, int& index) {
        std::vector<std::string> tokens;
        std::string required_token = "";
        int text_size = text.size();
        while(index < text_size) {
            if(WHITESPACE(index)) {
                index++;
                continue;
            }
            required_token = parse_variable(text, index);
            if(required_token != "") {
                tokens.emplace_back(required_token);
            }
            while(WHITESPACE(index)) {
                index++;
            }
            if(text[index] == ',' || index == text_size) {
                tokens.emplace_back(null_val);
                index++;
                continue;
            }
            if(text[index] == '=') {
                index++;
                while(WHITESPACE(index)) {
                    index++;
                }
                required_token = parse_value(text, index);
                tokens.emplace_back(required_token);
                index++;

                while(WHITESPACE(index)) {
                    index++;
                }
                if(match(index, text, target_operator, false)) {
                    END;
                }
                if(index == text_size) {
                    break;
                }
                if(text[index] == ',') {
                    index++;
                    continue;
                }
                else {
                    std::cout << index << " comma expected\n"; END;
                }
            }
            else {
                END;
            }
            index++;
        }

        return tokens;
    }

    std::vector<std::string> parse_values(const std::string& text, int& index) {
        int text_size = text.size();
        std::vector<std::string> values;
        std::string required_token = "";
        while(index < text_size && !match(index, text, target_operator, false)) {
            if(WHITESPACE(index)) {
                index++;
                continue;
            }
            required_token = parse_value(text, index);
            values.emplace_back(required_token);
            index++;
        }
        return values;
    }

    Token parse(const std::string& text, Config* config) {
        Token token;
        int index = 0;
        int text_size = text.size();

        int begin = 0;
        int end = 0;
        std::string required_token;

        while(index < text_size) {
            if(WHITESPACE(index)) {
                index++;
                continue;
            }
            if(index == 0) {
                if(!config->head) {
                    END;
                }
                begin = index;
                while(!WHITESPACE(index) && index != text_size) {
                    index++;
                }
                end = index;
                required_token = text.substr(begin, end - begin);
                token.head = required_token;
            }

            if(token.head == "var" || token.head == "const" || token.head == "static") {
                if(!config->init_list) {
                    END;
                }
                index++;
                while(WHITESPACE(index)) {
                    index++;
                }
                token.init = parse_init(text, index);
                continue;
            }
            else {
                token.values = parse_values(text, index);
                if(config->single_arg && token.values.size() > 1) {
                    END;
                }
            }

            if(match(index, text, target_operator)) {
                if(!config->target) {
                    END;
                }
                begin = index;
                while(text[index] != '\0') {
                    if(WHITESPACE(index)) {
                        begin++;
                    }
                    index++;
                }
                end = index;
                required_token = text.substr(begin, end - begin + 1);
                token.target = required_token;
            }

            required_token = parse_value(text, index);
            index++;
        }

        return token;
    }
};
