#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

class Value;
struct Token;

using Memory = std::unordered_map<std::string, Value*>;
using Table = std::unordered_map<std::string, std::string>;

class Result {
    private:
    public:
        double* number = nullptr;
        std::string* string = nullptr;
        std::vector<std::string>* list = nullptr;
        std::unordered_map<std::string, std::string>* dict = nullptr;

        Result(std::string);
        Result operator[](int);
        Result operator[](std::string);
        ~Result();
};

class Kal {
    private:
        Memory k_memory;

    public:
        std::string exec(std::string, Table = {});
        double number(std::string);
        std::string string(std::string);
        std::string list(std::string, int);
        std::string dict(std::string, std::string);
        ~Kal();
};