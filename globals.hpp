#pragma once

#include <string>
#include <stack>
#include <utility>
#include <unordered_map>
#include <chrono>

class Value;
using Memory = std::unordered_map<std::string, Value*>;
using ScopeTable = std::unordered_map<std::string, int>;
using CallStack = std::stack<std::pair<std::string, int>>;
using DeferStack = std::stack<std::pair<std::string, int>>;
using InertTable = std::unordered_map<std::string, std::string>;
using InertHit = std::unordered_map<std::string, bool>;

struct Globals {
    std::chrono::time_point<std::chrono::high_resolution_clock> clock_start;
    int depth;
    bool error_exit = true;
    std::string* current_line = nullptr;
    Memory memory;
    ScopeTable scope;
    DeferStack defer_stack;
    CallStack call_stack;
    InertTable inert_table;
    InertHit inert_hit;
};

Globals globals;

#include "types.hpp"

enum Type {
    VAR,
    INERT
};

namespace VarTable {
    std::string print(std::string, Globals&);
    Value* get(std::string, std::vector<std::string>, bool, bool, bool, Globals&);
    void set(std::string var, std::string data, Value* data_ptr, Type type, bool disallow_copy, int depth, bool allow_shadowing, Globals& globals);
}
