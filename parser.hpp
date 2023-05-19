#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>

#include "expr_parser.hpp"

#define WHITESPACE(position) (text[position] == ' ' || text[position] == '\t' || text[position] == '\n')

namespace parser {
    std::string general_delimiter = ",";
    std::string str_delimiter = "\"";
    std::string list_open = "[";
    std::string list_close = "]";
    std::string target_operator = "->";
    std::string assignment_operator = "=";

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
        return text.substr(begin, index - begin);
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

    std::string parse_value(const std::string& text, int& index) {
        int begin = index + 1;
        index++;
        while(text[index] != '`') {
            index++;
        }
        int end = index - 1;
        std::string expr = text.substr(begin, end - begin + 1);
        return std::to_string(eval(expr));
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
            }
        }

        return elements;
    }

    std::vector<std::string> parse(const std::string& text, bool direct = false) {
        std::vector<std::string> tokens;
        int index = 0;
        int text_size = text.size();

        int begin = 0;
        int end = 0;
        bool inside_string = false;
        std::string required_token;

        while(index < text_size) {
            if(WHITESPACE(index)) {
                index++;
                continue;
            }

            if(index == 0) {
                begin = index;
                while(!WHITESPACE(index)) {
                    index++;
                }
                end = index;
                required_token = text.substr(begin, end - begin);
                tokens.emplace_back(required_token);
            }

            if(is_num(text[index])) {
                begin = index;
                while(is_num(text[index])) {
                    index++;
                }
                end = index;
                required_token = text.substr(begin, end - begin);
                tokens.emplace_back(required_token);
            }

            if(text[index] == '=') {
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
                index += (end - begin);

                /*begin = index;
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
                tokens.emplace_back(required_token);*/
            }

            if(match(index, text, target_operator)) {
                begin = index;
                while(text[index] != '\0') {
                    if(WHITESPACE(index)) {
                        begin++;
                    }
                    index++;
                }
                end = index;
                required_token = text.substr(begin, end - begin + 1);
                tokens.emplace_back(required_token);
            }

            if(match(index, text, assignment_operator)) {
                while(WHITESPACE(index)) {
                    index++;
                }
                if(text[index] == '"') {
                    required_token = parse_string(text, index);
                }
                if(text[index] == '`') {
                    required_token = parse_value(text, index);
                }
                else if(is_number(text[index])) {
                    required_token = parse_number(text, index);
                }
                tokens.emplace_back(required_token);
                //index++;
            }

            if(text[index] == '[') {
                tokens.emplace_back(extract_list(text, index));
            }

            if(text[index] == '`') {
                tokens.emplace_back(parse_value(text, index));
            }

            if(text[index] == '"') {
                tokens.emplace_back(parse_string(text, index));
            }

            index++;
        }

        return tokens;
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
