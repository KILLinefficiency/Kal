#pragma once

#define END std::cout << "syntax error\n"; exit(1)

#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>

#include "config.hpp"
#include "lib/lib_string.hpp"

#define WHITESPACE(text, position) (text[position] == ' ' || text[position] == '\t' || text[position] == '\n' || text[position] == '\r')

std::string eval(std::string expr);

namespace parser {
    // for now.
    std::string extract_list(const std::string&, char, int&);
    std::string parse_value(const std::string&, int&);
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

    std::string parse_variable(const std::string& text, int& index, bool with_sub = true) {
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
                if(text[index] == '\0' || text[index] == '=' || text[index] == ',' || WHITESPACE(text, index) || text[index] == '[') {
                    break;
                }
                //END;
            }
            text_pos++;
        }
        int end = index;
        //index--;
        std::string required_string = text.substr(begin, end - begin);
        if(text[index] == '[' && with_sub) {
            while(text[index] == '[') {
                std::string sub_body = extract_list(text, '[', index);
                index++;
                required_string += sub_body;
            }
        }
        return required_string;
    }

    std::string extract_list(const std::string& text, char open, int& index) {
        char close = '\0';
        if(open == '[') {
            close = ']';
        }
        else if(open == '(') {
            close = ')';
        }
        int depth = 0;
        int start = index;
        while(text[index] != close || depth != 0) {
            if(text[index] == open) {
                depth++;
            }
            if(text[index + 1] == close) {
                depth--;
            }
            index++;
        }
        return text.substr(start, index - start + 1);
    }

    // Merge the below two versions into one if possible. Also I might need same type of function to extract $[] and $(), so it's better to have one.
    std::string extract_fstr(const std::string& text, int& index) {
        index++;
        std::string required_fstr = 'f' + extract_list(text, '[', index);
        return required_fstr;
    }

    std::string extract_dict(const std::string& text, int& index) {
        index++;
        std::string required_dict = '#' + extract_list(text, '(', index);
        return required_dict;
    }


    void skip_string(std::string& text, int& index) {
        index++;
        while(text[index] != '"') {
            index++;
        }
    }

    void skip_list(std::string& text, char open, int& index) {
        char close = '\0';
        if(open == '[') {
            close = ']';
        }
        else if(open == '(') {
            close = ')';
        }
        int depth = 0;
        while(text[index] != close || depth != 0) {
            if(text[index] == '"') {
                skip_string(text, index);
            }
            if(text[index] == open) {
                depth++;
            }
            else if(text[index + 1] == close) {
                depth--;
            }
            index++;
        }
    }

    void skip_dict(std::string& text, int& index) {
        index++;
        skip_list(text, '(', index);
    }

    std::vector<std::string> parse_fstr(const std::string& text, int& index) {
        index++;
        std::vector<std::string> values;
        std::string contents = extract_list(text, '[', index);
        contents = contents.substr(1, contents.size() - 2);
        int begin = 0;
        int end = 0;
        int i = 0;
        int size = contents.size();
        int last_i = size - 1;
        std::string item;
        while(i < size) {
            while(contents[begin] == ' ' || contents[begin] == '\t' || contents[begin] == '\n') {
                begin++;
            }
            if(contents[i] == '"') {
                skip_string(contents, i);
            }
            else if(contents[i] == '[' || contents[i] == '(') {
                skip_list(contents, contents[i], i);
            }
            if(contents[i] == ',' || i == last_i) {
                end = i;
                if(i == last_i) {
                    end++;
                }
                while(contents[end - 1] == ' ' || contents[end - 1] == '\t' || contents[end - 1] == '\n') {
                    end--;
                }
                item = contents.substr(begin, end - begin);
                values.emplace_back(item);
                begin = i + 1;
            }
            i++;
        }
        return values;
    }

    std::vector<std::string> parse_list(std::string& text, int& index) {
        std::vector<std::string> items;
        std::string element;
        int depth = 0;
        bool inside_string = false;
        index++;
        while(WHITESPACE(text, index)) {
            index++;
        }
        int begin = index;
        int end = index;
        while(text[index] != ']' || depth != 0 || inside_string) {
            if(text[index] == '"') {
                inside_string = !inside_string;
            }
            if(inside_string) {
                index++;
                continue;
            }
            if(match(index, text, "f[", false)) {
                index++;
                skip_list(text, '[', index);
            }
            if(match(index, text, "#(", false)) {
                index++;
                skip_list(text, '(', index);
            }
            if(text[index] == '[') {
                skip_list(text, text[index], index);
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
            while(WHITESPACE(text, end - 1)) {
                end--;
            }
            element = text.substr(begin, end - begin);
            index++;
            while(WHITESPACE(text, index)) {
                index++;
            }
            begin = index;
            items.emplace_back(element);
        }

        return items;
    }
    /*std::unordered_map<std::string, std::string> parse_list(std::string& text, int index = 0) {
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
    }*/

    std::string parse_value(const std::string& text, int& index) {
        std::string required_token = "";

        if(is_num(text[index])) {
            required_token = parse_number(text, index);
        }
        else if(text[index] == '$') {
            required_token = parse_variable(text, index);
        }
        else if(text[index] == '[') {
            required_token = extract_list(text, '[', index);
        }
        else if(text[index] == '`') {
            required_token = parse_expr(text, index);
        }
        else if(text[index] == '"') {
            required_token = parse_string(text, index);
        }
        else if(match(index, text, "f[", false)) {
            required_token = extract_fstr(text, index);
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

    std::vector<std::string> parse_init(std::string text, int& index, bool for_dict = false) {
        std::vector<std::string> tokens;
        std::string assign_op = "=";
        std::string required_token = "";
        int text_size = text.size();
        while(index < text_size) {
            if(WHITESPACE(text, index)) {
                index++;
                continue;
            }
            if(for_dict) {
                assign_op = "->";
            }
            if(for_dict && text[index] == '"') {
                required_token = parse_string(text, index);
                index++;
            }
            else {
                required_token = parse_variable(text, index);
            }
            if(required_token != "") {
                tokens.emplace_back(required_token);
            }
            while(WHITESPACE(text, index)) {
                index++;
            }
            if(index < text_size && match(index, text, target_operator, false) && !for_dict) {
                END;
            }
            if(text[index] == ',' || index == text_size) {
                tokens.emplace_back(null_val);
                index++;
                continue;
            }
            //if(text[index] == '=') {
            if(match(index, text, assign_op)) {
                //index++;
                while(WHITESPACE(text, index)) {
                    index++;
                }
                int begin = index;
                while(text[index] != ',' && index != text_size) {
                    if(index < text_size && match(index, text, target_operator, false) && !for_dict) {
                        END;
                    }
                    if(text[index] == '"') {
                        skip_string(text, index);
                    }
                    if(text[index] == '[' || text[index] == '(') {
                        skip_list(text, text[index], index);
                    }
                    if(match(index, text, "#(", false)) {
                        skip_dict(text, index);
                    }
                    index++;
                }
                int end = index;
                while(WHITESPACE(text, end - 1)) {
                    end--;
                }
                required_token = text.substr(begin, end - begin);
                tokens.emplace_back(required_token);
            }
            else {
                std::cout << index << " " << text[index] << std::endl;
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
            if(WHITESPACE(text, index)) {
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
            if(WHITESPACE(text, index)) {
                index++;
                continue;
            }
            if(index == 0) {
                if(!config->head) {
                    END;
                }
                begin = index;
                while(!WHITESPACE(text, index) && index != text_size) {
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
                while(WHITESPACE(text, index)) {
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
                    if(WHITESPACE(text, index)) {
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
