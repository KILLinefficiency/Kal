#pragma once

#include <iostream>
#include <unordered_map>

class Value;
struct Token;

using Memory = std::unordered_map<std::string, Value*>;
using Table = std::unordered_map<std::string, std::string>;

class Kal {
    private:
        Memory k_memory;

    public:
        Value* exec(std::string, Table = {});
        ~Kal();
};