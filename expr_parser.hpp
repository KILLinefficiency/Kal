#pragma once

#include <sstream>
#include <stack>
#include <queue>
#include <iostream>
#include <cmath>
#include <sstream>

#include "parser.hpp"
#include "errors.hpp"
#include "lib/lib_math.hpp"
#include "lib/lib_string.hpp"

#define SET_CURRENT_OP(X) else if(match(expr, X, index)) current_op = X

std::string str_add(const std::string& x, const std::string& y) {
    std::string xy;
    xy = x.substr(0, x.size() - 1);
    xy += y.substr(1, y.size() - 1);
    return xy;
}

std::string str_mul(std::string& x, double d_times) {
    std::stringstream x_times;
    x_times << '"';
    int times = int(d_times);
    x = x.substr(1, x.size() - 2);
    while(times--) {
        x_times << x;
    }
    x_times << '"';
    return x_times.str();
}

namespace ops {
    const std::string negative = "n",
                      log_not = "!",
                      bit_not = "~",
                      exp = "**",
                      mod = "%",
                      div = "/",
                      mul = "*",
                      sub = "-",
                      add = "+",
                      l_shift = "<<",
                      r_shift = ">>",
                      lt = "<",
                      lte = "<=",
                      gt = ">",
                      gte = ">=",
                      eq = "==",
                      neq = "!=",
                      bit_and = "&",
                      bit_xor = "^",
                      bit_or = "|",
                      log_and = "&&",
                      log_or = "||",
                      if_null = "??",
                      t_if = "?",
                      t_else = ":",
                      left = "(",
                      right = ")";
}

bool match(std::string& text, std::string pattern, int& index) {
    int pattern_len = pattern.size();
    bool found = pattern == text.substr(index, pattern_len);
    if(found) {
        index += (pattern_len - 1);
    }
    return found;
}

int order(std::string op) {
    if (op == ops::if_null)                                                      return 13;
    else if (op == ops::negative || op == ops::log_not || op == ops::bit_not)    return 12;
    else if (op == ops::exp)                                                     return 11;
    else if (op == ops::mul || op == ops::div || op == ops::mod)                 return 10;
    else if (op == ops::add || op == ops::sub)                                   return 9;
    else if (op == ops::l_shift || op == ops::r_shift)                           return 8;
    else if (op == ops::lt || op == ops::lte || op == ops::gt || op == ops::gte) return 7;
    else if (op == ops::eq || op == ops::neq)                                    return 6;
    else if (op == ops::bit_and)                                                 return 4;
    else if (op == ops::bit_xor)                                                 return 4;
    else if (op == ops::bit_or)                                                  return 3;
    else if (op == ops::log_and)                                                 return 2;
    else if (op == ops::log_or)                                                  return 1;
    else if (op == ops::t_if || op == ops::t_else)                               return -1;
    return 0;
}

double mod(double x, double y) {
    x = x < 0 ? -x : x;
    y = y < 0 ? -y : y;
    while(x >= y) {
        x -= y;
    }
    x = x < 0 ? -x : x;
    return x;
}

std::string if_null(std::string& first, std::string& second) {
    if(first == parser::null_val) {
        return second;
    }
    return first;
}

/*
std::string fstr(const std::string& text) {
    int index = 0;
    std::vector<std::string> values = parser::parse_fstr(text, index);
    std::string& head = values[0];
    int size = values.size();
    if(size == 1) {
        return head;
    }
    std::stringstream fstring;
    std::string item;
    int i = 0;
    int count = 0;
    int args = size - 1;
    int head_size = head.size();
    while(i < head_size) {
        if(parser::match(i, head, "{}") && count < args) {
            item = eval(values[count + 1]);
            if(lib::is_string(item)) {
                item = lib::resolve_string(item);
            }
            fstring << item;
            count++;
            continue;
        }
        fstring << head[i];
        i++;
    }
    return fstring.str();
}
*/
std::string fstr(const std::string& text) {
    int index = 0;
    std::vector<std::string> values = parser::parse_fstr(text, index);
    int size = values.size();
    if(size == 0) {
        return "";
    }
    values[0] = eval(values[0]);
    std::string& head = values[0];
    if(size == 1) {
        return head;
    }
    std::string item;
    std::string fstring = "";
    int i = 0;
    int count = 0;
    int begin = 0;
    int args = size - 1;
    int head_size = head.size();
    int last_i = head_size - 1;
    while(i < head_size) {
        if(parser::match(i, head, "{}", false) && count < args) {
            fstring += head.substr(begin, i - begin);
            item = eval(values[count + 1]);
            if(lib::is_string(item)) {
                item = lib::resolve_string(item);
            }
            fstring += item;
            i += 2;
            begin = i;
            count++;
            continue;
        }
        else if(i == last_i) {
            fstring += head.substr(begin, i - begin + 1);
        }
        i++;
    }
    return fstring;
}

/// dummy function.
/// Sample expression: $name[$hey[$inner[1 + 2 * 3] + 5] + 15] * $pi
///                    $name[ $hey[ $inner[ 1 + 2 * 3 ] + 5 ] + 15 ] * ($pi + 0.9)
std::string get_val(std::string var) {
    if(var == "$inner[7]") { return "10"; }
    else if(var == "$hey[15]") { return "5"; }
    else if(var == "$name[20]") { return "100"; }
    else if(var == "$pi") { return "3.14"; }
    return "";
}
///

std::string expand_var(std::string var) {
    int index = 0;
    std::string variable = parser::parse_variable(var, index, false);
    int size = var.size();
    if(var[index] == '[') {
        index++;
        std::string sub = var.substr(index, size - index - 1);
        sub = eval(sub);
        variable += ("[" + sub + "]");
    }
    // get real values here.
    /// dummy call.
    variable = get_val(variable);
    ///
    return variable;
}

std::string eval(std::string expr) {
    std::string result;
    std::string current_op = "";
    std::queue<std::string> rpn;
    std::stack<std::string> operators;

    int index = 0;
    int expr_len = expr.size();
    while(index < expr_len) {
        if(expr[index] == ops::negative[0] || expr[index] == ops::log_not[0] || expr[index] == ops::bit_not[0]) {
            int next_index = index + 1;
            while(expr[next_index] == ' ' || expr[next_index] == '\t' || expr[next_index] == '\n') {
                next_index++;
            }
            if(expr[index] == expr[next_index]) {
                index = next_index + 1;
                continue;
            }
        }
        if(expr[index] == ' ' || expr[index] == '\t' || expr[index] == '\n') {
            index++;
            continue;
        }
        else if(expr[index] == '"') {
            std::string value = parser::parse_string(expr, index);
            rpn.push(value);
            index++;
            continue;
        }
        else if(expr[index] == '$') {
            std::string var = parser::parse_variable(expr, index);
            std::string val = expand_var(var);
            rpn.push(val);
        }
        else if(parser::match(index, expr, "f[", false)) {
            std::string line = parser::extract_fstr(expr, index);
            rpn.push(fstr(line));
        }
        else if((expr[index] >= '0' && expr[index] <= '9') || expr[index] == '.') {
            int begin = index;
            while((expr[index] >= '0' && expr[index] <= '9') || expr[index] == '.') {
                index++;
            }
            std::string value = expr.substr(begin, index - begin);
            rpn.push(value);
            index--;
        }
        else if(match(expr, parser::null_val, index)) {
            rpn.push(parser::null_val);
            index++;
            continue;
        }
        else if(match(expr, ops::left, index)) {
            operators.push(ops::left);
        }
        else if(match(expr, ops::right, index)) {
            while(!operators.empty() && operators.top() != ops::left) {
                rpn.push(operators.top());
                operators.pop();
            }
            operators.pop();
        }

        else {
            if(match(expr, ops::exp, index)) current_op = ops::exp;
            SET_CURRENT_OP(ops::l_shift);
            SET_CURRENT_OP(ops::r_shift);
            SET_CURRENT_OP(ops::lte);
            SET_CURRENT_OP(ops::gte);
            SET_CURRENT_OP(ops::eq);
            SET_CURRENT_OP(ops::neq);
            SET_CURRENT_OP(ops::log_and);
            SET_CURRENT_OP(ops::log_or);
            SET_CURRENT_OP(ops::log_not);
            SET_CURRENT_OP(ops::bit_not);
            SET_CURRENT_OP(ops::mod);
            SET_CURRENT_OP(ops::div);
            SET_CURRENT_OP(ops::mul);
            SET_CURRENT_OP(ops::sub);
            SET_CURRENT_OP(ops::add);
            SET_CURRENT_OP(ops::lt);
            SET_CURRENT_OP(ops::gt);
            SET_CURRENT_OP(ops::bit_and);
            SET_CURRENT_OP(ops::bit_xor);
            SET_CURRENT_OP(ops::bit_or);
            SET_CURRENT_OP(ops::if_null);
            SET_CURRENT_OP(ops::negative);
            SET_CURRENT_OP(ops::t_if);
            SET_CURRENT_OP(ops::t_else);

            while(!operators.empty() && operators.top() != ops::left && order(operators.top()) >= order(current_op)) {
                rpn.push(operators.top());
                operators.pop();
            }
            operators.push(current_op);
        }

        index++;
    }


    while(!operators.empty()) {
        rpn.push(operators.top());
        operators.pop();
    }

    std::string a, b;
    double x = 0, y = 0;
    std::string token;
    std::stack<std::string> numbers;
    while(!rpn.empty()) {
        token = rpn.front();
        rpn.pop();
        if(!order(token)) {
            numbers.push(token);
        }
        else if(order(token) == 12) {
            y = std::stod(numbers.top());
            numbers.pop();
            if(token == "n") {
                y = -y;
            }
            else if(token == "!") {
                y = !y;
            }
            else if(token == "~") {
                y = ~long(y);
            }
            numbers.push(std::to_string(y));
        }
        else {
            if(token == "?") {
                b = numbers.top();
                numbers.pop();
                a = numbers.top();
                numbers.pop();

                if(a != "0") {
                    numbers.push(b);
                }
                else {
                    numbers.push(a);
                }
                continue;
            }
            if(token == ":") {
                b = numbers.top();
                numbers.pop();
                a = numbers.top();
                numbers.pop();
                if(a == "0") {
                    numbers.push(b);
                } else {
                    numbers.push(a);
                }
                continue;
            }

            b = numbers.top();
            numbers.pop();
            a = numbers.top();
            numbers.pop();

            if(token == "??") {
                numbers.push(if_null(a, b));
                continue;
            }
            if(lib::is_string(a) && lib::is_string(b)) {
                if(token == "+") {
                    numbers.push(str_add(a, b));
                }
                else if(token == "==") {
                    numbers.push(std::to_string(a == b));
                }
                else if(token == "!=") {
                    numbers.push(std::to_string(a != b));
                }
                else {
                    std::string line = a + " " + token + " " + b;
                    errors::invalid_operation_error(line, "strings", token, a, b);
                }
                continue;
            }
            else if(token == "*" && lib::is_string(a) && !lib::is_string(b)) {
                numbers.push(str_mul(a, std::stod(b)));
                continue;
            }
            else if(token == "*" && !lib::is_string(a) && lib::is_string(b)) {
                numbers.push(str_mul(b, std::stod(a)));
                continue;
            }
            else if(a == parser::null_val || b == parser::null_val) {
                if(token == "==") {
                    numbers.push(std::to_string(a == b));
                }
                else if(token == "!=") {
                    numbers.push(std::to_string(a != b));
                }
                else {
                    std::string line = a + " " + token + " " + b;
                    errors::invalid_operation_error(line, "values", token, a, b);
                }
                continue;
            }

            x = std::stod(a);
            y = std::stod(b);

            if     (token == "+")   numbers.push(std::to_string(x + y));
            else if(token == "-")   numbers.push(std::to_string(x - y));
            else if(token == "*")   numbers.push(std::to_string(x * y));
            else if(token == "/")   numbers.push(std::to_string(x / y));
            else if(token == "%")   numbers.push(std::to_string(mod(x, y)));
            else if(token == "**")  numbers.push(std::to_string(pow(x, y)));
            else if(token == "<<")  numbers.push(std::to_string(long(x) << long(y)));
            else if(token == ">>")  numbers.push(std::to_string(long(x) >> long(y)));
            else if(token == "<")   numbers.push(std::to_string(x < y));
            else if(token == ">")   numbers.push(std::to_string(x > y));
            else if(token == "<=")  numbers.push(std::to_string(x <= y));
            else if(token == ">=")  numbers.push(std::to_string(x >= y));
            else if(token == "==")  numbers.push(std::to_string(x == y));
            else if(token == "!=")  numbers.push(std::to_string(x != y));
            else if(token == "&")   numbers.push(std::to_string(long(x) & long(y)));
            else if(token == "^")   numbers.push(std::to_string(long(x) ^ long(y)));
            else if(token == "|")   numbers.push(std::to_string(long(x) | long(y)));
            else if(token == "&&")  numbers.push(std::to_string(x && y));
            else if(token == "||")  numbers.push(std::to_string(x || y));
        }
    }
    result = lib::trim_num(numbers.top());
    numbers.pop();
    return result;
}