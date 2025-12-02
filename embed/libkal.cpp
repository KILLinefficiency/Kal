#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>

#include "../config.hpp"
#include "../var.hpp"
#include "../exec.hpp"
#include "../preprocessor.hpp"
#include "../expr_parser.hpp"

#include "kal.hpp"

using Table = std::unordered_map<std::string, std::string>;

bool is_alpha(char& current) {
    return ((current >= 'a' && current <= 'z') || ((current >= 'A') && (current <= 'Z')));
}

bool is_num(char& current) {
    return (current >= '0') && (current <= '9');
}

bool is_underscore(char& current) {
    return current == '_';
}

void skip_var(std::string& code, int& index) {
    while(is_alpha(code[index]) || is_num(code[index]) || is_underscore(code[index])) {
        index++;
    }
}

std::string format_code(std::string& code, Table& table) {
    std::stringstream formatted_code;

    int size = code.size();
    int index = 0, begin = 0, end = 0;
    std::string prev = "", token = "", next = "", obtained_token = "";

    while(index < size) {
        if(code[index] == '$' && (is_alpha(code[index + 1]) || is_underscore(code[index + 1]))) {
            end = index;
            index++;
            prev = code.substr(begin, end - begin);
            formatted_code << prev;
            begin = index;
            skip_var(code, index);
            token = code.substr(begin, index - begin);
            obtained_token = table[token];
            if(obtained_token == "" && table.find(token) != table.end()) {
                std::cerr << "Variable: `" << token << "` not specified.\n";
                exit(1);
            }
            formatted_code << obtained_token;
            begin = index;
        }
        if(index == size - 1) {
            next = code.substr(begin);
            formatted_code << next;
        }
        index++;
    }

    return formatted_code.str();
}

Result::Result(std::string value) {
    int begin = 0;
    if(is_num(value[0])) {
        number = new double(std::stod(parser::parse_number(value, begin)));
    }
    else if(value[0] == '"') {
        string = new std::string(parser::parse_string(value, begin));
    }
    else if(value[0] == '[') {
        list = new std::vector<std::string>(parser::parse_list(value, begin));
    }
    else if(value[0] == '#' && value[1] == '(') {
        dict = new std::unordered_map<std::string, std::string>();
        std::vector<std::string> dict_values = parse_map(value, begin);
        int size = dict_values.size();
        for(int idx = 0; idx < size; idx += 2) {
            (*dict)[dict_values[idx]] = dict_values[idx + 1];
        }
    }
}

Result Result::operator[](int index) {
    if(list == nullptr) {
        std::cout << "List not found.\n";
        exit(1);
    }
    int size = list->size();
    if(index >= size) {
        std::cout << "List out of bounds.\n";
    }
    return Result((*list)[index]);
}

Result Result::operator[](std::string key) {
    if(dict == nullptr) {
        std::cout << "Dict not found.\n";
        exit(1);
    }
    return Result((*dict)[key]);
}

std::ostream& operator<<(std::ostream& out, Result result) {
    out << "Result(";
    if(result.number != nullptr) {
        out << *(result.number);
    }
    else if(result.string != nullptr) {
        out << *(result.string);
    }
    out << ")";
    return out;
}

Result::~Result() {
    if(number) {
        delete number;
        number = nullptr;
    }
    else if(string) {
        delete string;
        string = nullptr;
    }
    else if(list) {
        delete list;
        list = nullptr;
    }
    else if(dict) {
        delete dict;
        dict = nullptr;
    }
}

std::string Kal::exec(std::string code, Table table) {
    if(!table.empty()) {
        code = format_code(code, table);
    }

    std::vector<std::string> lines = preproc::preprocess(code);
    std::vector<Token> tokens = lexer::tokenize(lines);
    Value* ret_val = line_exec(tokens, false, false, true, k_memory);

    if(ret_val == nullptr) {
        return "";
    }

    std::string value = ret_val->print();
    delete ret_val;
    return value;
}

double Kal::number(std::string value) {
    if(is_num(value[0])) {
        return std::stod(value);
    }

    return 0;
}

std::string Kal::string(std::string value) {
    int size = value.size();
    if(value[0] == '"' && value[size - 1] == '"') {
        return value.substr(1, size - 2);
    }

    return "";
}

std::string Kal::list(std::string value, int index) {
    int begin = 0;
    std::vector<std::string> kal_list = parser::parse_list(value, begin);

    int size = kal_list.size();
    if(index < size) {
        return kal_list[index];
    }

    return "";
}

std::string Kal::dict(std::string value, std::string key) {
    int begin = 0;
    std::vector<std::string> kal_dict = parse_map(value, begin);

    int size = kal_dict.size();
    for(int index = 0; index < size; index += 2) {
        if(kal_dict[index] == key) {
            return kal_dict[index + 1];
        }
    }

    return "";
}

Kal::~Kal() {
    VarTable::gc(0, k_memory);
}