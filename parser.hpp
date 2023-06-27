#pragma once

#define END std::cout << "syntax error\n"; exit(1)

#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>

#include "config.hpp"
#include "expr_parser.hpp"

#define WHITESPACE(position) (text[position] == ' ' || text[position] == '\t' || text[position] == '\n')

namespace parser {
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

    bool match(int& index, std::string text, const std::string pattern) {
        int pattern_len = pattern.size();
        std::string found_pattern = text.substr(index, pattern_len);
        bool matched = pattern == found_pattern;
        if(matched) {
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
        return std::to_string(eval(expr));
    }

    std::string parse_variable(const std::string& text, int& index) {
        int begin = index;
        index++;
        while(is_alpha(text[index])) {
            index++;
        }
        int end = index;
        index--;
        std::string required_string = text.substr(begin, end - begin);
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
                std::cout << element_idx << ": " << element << std::endl;;
            }
        }

        return elements;
    }

    std::string parse_value(const std::string& text, int& index/*, std::vector<std::string>& tokens*/) {
        std::string required_token = "";

        if(is_num(text[index])) {
            /*begin = index;
            while(is_num(text[index])) {
                index++;
            }
            end = index;
            required_token = text.substr(begin, end - begin);*/
            required_token = parse_number(text, index);
        }

        if(text[index] == '$') {
            required_token = parse_variable(text, index);
        }

        if(text[index] == '[') {
            required_token = extract_list(text, index);
        }

        if(text[index] == '`') {
            required_token = parse_expr(text, index);
        }

        if(text[index] == '"') {
            required_token = parse_string(text, index);
        }
        if(match(index, text, null_val)) {
            required_token = null_val;
        }

        return required_token;
    }

    std::vector<std::string> parse_init(const std::string& text, int& index/*, std::vector<std::string>& tokens*/) {
        int begin = index;
        int end = index;
        int text_size = text.size();
        std::vector<std::string> tokens;
        std::string required_token = "";
        while(index < text_size) {
            bool key_val = false;
            /*while(WHITESPACE(index)) {
                index++;
            }*/
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
                //std::cout << "here: " << index << text[index] << "\n";
                while(WHITESPACE(index)) {
                    index++;
                }
            }
            if(text[index] == ',') {
                if(!key_val) {
                    int var_end = index;
                    while(WHITESPACE(var_end - 1)) {
                        var_end--;
                    }
                    var_end--;
                    int var_start = var_end;
                    while(is_alpha(text[var_start - 1])) {
                        var_start--;
                    }
                    required_token = text.substr(var_start, var_end - var_start + 1);
                    tokens.emplace_back(required_token);
                    tokens.emplace_back(null_val);
                }
                index++;
                while(WHITESPACE(index)) {
                    index++;
                }
                begin = index;
                index++;
                continue;
            }

            index++;
        }

        return tokens;
    }

    Token parse(const std::string& text, Config* config) {
        //bool done = false;
        Token token;
        std::vector<std::string> tokens;
        int index = 0;
        int text_size = text.size();

        int begin = 0;
        int end = 0;
        //bool inside_string = false;
        std::string required_token;

        while(index < text_size) {
            if(WHITESPACE(index)) {
                index++;
                continue;
            }
            if(index == 0) {
                /*if(text[index] == '$') {
                    int begin = index;
                    int assignment = index;
                    while(text[index] != '=') {
                        index++;
                    }
                    int end = index;
                    while(WHITESPACE(end - 1)) {
                        end--;
                    }
                    required_token = text.substr(begin, end - begin);
                    tokens.emplace_back(required_token);
                    if(text[index] == '=') {
                        index++;
                        assignment = index;
                        while(WHITESPACE(assignment)) {
                            assignment++;
                        }
                        index = assignment;
                        while(text[index] != '\0') {
                            index++;
                        }

                        required_token = text.substr(assignment, index - assignment);
                        tokens.emplace_back(required_token);
                    }
                }
                else {*/
                    begin = index;
                    while(!WHITESPACE(index) && index != text_size) {
                        index++;
                    }
                    end = index;
                    required_token = text.substr(begin, end - begin);
                    //tokens.emplace_back(required_token);
                    token.head = required_token;
                //}
            }

            if(token.head == "var" || token.head == "const" || token.head == "static") {
                index++;
                while(WHITESPACE(index)) {
                    index++;
                }
                //std::cout << index << std::endl;
                token.init = parse_init(text, index);
                continue;
            }

            /*if(text[index] == '=' && config->init_list) {
                //std::cout << config->id << std::endl;
                end = index;
                while(!is_alpha(text[end])) {
                    end--;
                }
                begin = end;
                while(!WHITESPACE(begin - 1)) {
                    begin--;
                }
                required_token = text.substr(begin, end - begin + 1);
                tokens.emplace_back(required_token);
                //index += (end - begin - 1);
                index++;
                //std::cout << "later: " << index << std::endl;

                //
                begin = index;
                begin++;
                while(WHITESPACE(begin)) {
                    begin++;
                }
                end = begin;
                while(is_alpha(text[end])) {
                    end++;
                }
                index = end;
                required_token = text.substr(begin, end - begin);
                tokens.emplace_back(required_token);
                std::cout << required_token << std::endl;*/
            //}

            // perform check inside. (if not config->target: throw error)
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
                //tokens.emplace_back(required_token);
                token.target = required_token;
            }

            if(match(index, text, assignment_operator) && config->init_list) {
                while(WHITESPACE(index)) {
                    index++;
                }
                required_token = parse_value(text, index);
                tokens.emplace_back(required_token);
            }

            required_token = parse_value(text, index);
            tokens.emplace_back(required_token);
            index++;
        }

        /*for(std::string x : tokens) {
            std::cout << "{" << x << "}" << std::endl;
        }
        std::cout << "----------" << std::endl;*/

        return token;
    }
};

/*int main() {
    int index = 0;
    std::string text = R"(["hey", [1, 2, ["a", "b"]], [3, 4], "o"])";
    text = R"(["a", "b", "c"])";
    //text = R"(["a", "b", [1, 2, 3], [3.14], "c"])";
    text = R"([1, 2, 3])";
    //text = R"([[1, 2, 3, ["x", "y"]], "a", "b", "c"])";
    text = R"([1, [2, [3, [4, [5, [6]]]]]])";
    text = R"([[1, 2, 3], [4, 5, 6], [7, 8, 9,,],])";
    text = R"([[1, 2, 3]])";
    text = R"([[[[,1,]],]])";
    std::cout << "List: " << text << std::endl;
    std::vector<std::string> tokens = p.parse_list(text, index);

    std::string cmd = "var num: res = \"hey\"";
    std::vector<std::string> a = parse("var num: res = `(1 + 2) * 3`");
    for(std::string x : a) {
        std::cout << x << std::endl;
    }
    
    return 0;
}*/
