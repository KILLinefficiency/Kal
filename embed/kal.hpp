#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <unordered_map>

class Value;
class Result;
struct Token;
struct Globals;

using Memory = std::unordered_map<std::string, Value*>;
using Table = std::unordered_map<std::string, std::string>;
using ResultList = std::vector<Result>;
using ResultDict = std::unordered_map<std::string, Result>;

class Result {
    private:
    public:
        std::string value;
        bool is_null = false;
        double* number = nullptr;
        std::string* string = nullptr;
        std::vector<std::string>* list = nullptr;
        std::unordered_map<std::string, std::string>* dict = nullptr;

        Result();
        Result(std::string);
        Result(const Result&);
        ~Result();

        Result operator[](int);
        Result operator[](std::string);
        Result operator=(const Result&);

        double to_number();
        std::string to_string();
        std::vector<Result> to_list();
        std::unordered_map<std::string, Result> to_dict();
        bool to_null();

};

std::ostream& operator<<(std::ostream&, const Result&);

class Kal {
    private:
        Globals* k_globals;

    public:
        Kal();
        Result exec(std::string, Table = {});
        ~Kal();
};