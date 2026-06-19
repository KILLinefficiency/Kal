#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>

#include "../globals.hpp"
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

Result::Result() {}

Result::Result(std::string value) {
    this->value = value;
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
        std::vector<std::string> dict_values = parser::parse_map(value, begin);
        int size = dict_values.size();
        for(int idx = 0; idx < size; idx += 2) {
            (*dict)[dict_values[idx]] = dict_values[idx + 1];
        }
    }
    else if(value == "null") {
        is_null = true;
    }
}

Result::Result(const Result& result) {
    this->value = result.value;
    if(result.number) {
        number = new double(*result.number);
    }
    else if(result.string) {
        string = new std::string(*result.string);
    }
    else if(result.list) {
        list = new std::vector<std::string>(*result.list);
    }
    else if(result.dict) {
        dict = new std::unordered_map<std::string, std::string>(*result.dict);
    }
    else if(result.is_null) {
        is_null = true;
    }
}

Result Result::operator=(const Result& result) {
    this->value = result.value;
    if(result.number) {
        number = new double(*result.number);
    }
    else if(result.string) {
        string = new std::string(*result.string);
    }
    else if(result.list) {
        list = new std::vector<std::string>(*result.list);
    }
    else if(result.dict) {
        dict = new std::unordered_map<std::string, std::string>(*result.dict);
    }
    else if(result.is_null) {
        is_null = true;
    }
    return *this;
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

double Result::to_number() {
    if(number == nullptr) {
        std::cout << "Not a number.\n";
        exit(1);
    }
    return *number;
}

std::string Result::to_string() {
    if(string == nullptr) {
        std::cout << "Not a string.\n";
        exit(1);
    }
    int length = string->size();
    return string->substr(1, length - 2);
}

std::vector<Result> Result::to_list() {
    if(list == nullptr) {
        std::cout << "Not a list.\n";
        exit(1);
    }
    int list_size = list->size();
    std::vector<Result> result_list;
    result_list.reserve(list_size);
    for(std::string each : *list) {
        result_list.emplace_back(Result(each));
    }
    return result_list;
}

std::unordered_map<std::string, Result> Result::to_dict() {
    if(dict == nullptr) {
        std::cout << "Not a dict.\n";
        exit(1);
    }
    std::unordered_map<std::string, Result> dict_result;
    std::unordered_map<std::string, std::string>::iterator dict_itr;
    for(dict_itr = dict->begin(); dict_itr != dict->end(); dict_itr++) {
        dict_result[dict_itr->first] = Result(dict_itr->second);
    }
    return dict_result;
}

bool Result::to_null() {
    return is_null;
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

std::ostream& operator<<(std::ostream& out, const Result& result) {
    out << "Result(" << result.value << ")";
    return out;
}

Kal::Kal() {
    k_globals = new Globals;
}

Result Kal::exec(std::string code, Table table) {
    if(!table.empty()) {
        code = format_code(code, table);
    }

    std::vector<std::string> lines = preproc::preprocess(code);
    std::vector<Token> tokens = lexer::tokenize(lines);
    Value* ret_val = line_exec(tokens, false, false, true, *k_globals);

    if(ret_val == nullptr) {
        return Result("null");
    }

    std::string value = ret_val->print();
    delete ret_val;
    return Result(value);
}

Kal::~Kal() {
    VarTable::gc(*k_globals);
    delete k_globals;
}