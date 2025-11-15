#pragma once

#include <string>
#include <stack>
#include <utility>
#include <unordered_map>

#include "types.hpp"

int depth = 0;
std::unordered_map<std::string, Value*> memory;

enum Type {
    VAR,
    INERT
};

namespace VarTable {
    std::string print(std::string);
    Value* get(std::string, std::vector<std::string> = {}, bool = false, bool = false, bool = true, std::unordered_map<std::string, Value*>& = memory);
    void set(std::string var, std::string data, Value* data_ptr = nullptr, Type type = VAR, bool disallow_copy = false, int depth = 0, bool allow_shadowing = false, std::unordered_map<std::string, Value*>& memory = memory);
}