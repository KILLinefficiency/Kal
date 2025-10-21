#pragma once

#include <string>
#include <stack>
#include <utility>

int depth = 0;

std::stack<std::pair<std::string, int>> call_stack;

enum Type {
    VAR,
    INERT
};