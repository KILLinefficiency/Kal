#pragma once

#include <sstream>
#include <stack>
#include <queue>
#include <iostream>
#include <cmath>
#include <sstream>

#include "globals.hpp"
#include "types.hpp"
#include "parser.hpp"
#include "errors.hpp"
#include "lib/lib_math.hpp"
#include "lib/lib_string.hpp"

namespace VarTable {
    std::string print(std::string);
    Value* get(std::string, std::vector<std::string>, bool, bool, bool);
    void set(std::string, std::string, Value* data_ptrr = nullptr, Type type = VAR, bool disallow_copy = false, int depth = 0, bool allow_shadowing = false);
}

std::string eval(std::deque<std::string>);
bool compare(std::string, std::string);
Value* line_exec(std::vector<Token>&, bool, bool);

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

std::string to_str(std::string num) {
    if(num[0] == '"' && num[num.size() - 1] == '"') {
        return num;
    }
    return '"' + lib::trim_num(num) + '"';
}

std::string to_float(std::string num) {
    int size = num.size();
    if(num[0] == '"' && num[size - 1] == '"') {
        num = num.substr(1, size - 2);
    }
    return num;
}

std::string to_integer(std::string num) {
    num = to_float(num);
    size_t radix = num.find(".");
    if(radix != std::string::npos) {
        num = num.substr(0, radix);
    }
    return num;
}

std::string list_mul(std::string& list, double& times) {
    list = list.substr(1, list.size() - 2);
    list = lib::trim_trailing(list);
    std::string new_list = "[";
    std::string sep = ", ";
    while(times--) {
        new_list += (list + sep);
        if(times == 1) {
            sep = "";
        }
    }
    return new_list + "]";

}

namespace ops {
    const std::string negative = "0n",
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
                      right = ")",
                      as = "as",
                      walrus = ":=",
                      integer = "int",
                      floating = "float",
                      string = "str";
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
    if(op == ops::as)                                                            return 16;
    else if (op == ops::if_null)                                                 return 15;
    else if (op == ops::negative || op == ops::log_not || op == ops::bit_not)    return 14;
    else if (op == ops::exp)                                                     return 13;
    else if (op == ops::mul || op == ops::div || op == ops::mod)                 return 12;
    else if (op == ops::add || op == ops::sub)                                   return 11;
    else if (op == ops::l_shift || op == ops::r_shift)                           return 10;
    else if (op == ops::lt || op == ops::lte || op == ops::gt || op == ops::gte) return 9;
    else if (op == ops::eq || op == ops::neq)                                    return 8;
    else if (op == ops::bit_and)                                                 return 7;
    else if (op == ops::bit_xor)                                                 return 6;
    else if (op == ops::bit_or)                                                  return 5;
    else if (op == ops::log_and)                                                 return 4;
    else if (op == ops::log_or)                                                  return 3;
    else if (op == ops::t_if || op == ops::t_else)                               return 2;
    else if (op == ops::walrus)                                                  return 1;
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

std::string ternary_op(std::string& token, std::string& first, std::string& second) {
    bool condition = (first != "0" && first != "\"\"");

    if(token == ":") {
        condition = !condition;
    }
    if(condition) {
        return second;
    }

    return first;
}

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
            if(/*item[0] == '$'*/ parser::is_var(item) /*&& item[1] != '&'*/) {
                item = VarTable::print(item);
            }
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

std::string eval_indices(const std::string& text, int& index) {
    int size = text.size();
    std::string current, evaluated = "";
    while(index < size) {
        current = parser::extract_list(text, '[', index);
        std::string intermediate = eval(current.substr(1, current.size() - 2));
        if(/*intermediate[0] == '$'*/ parser::is_var(intermediate)/* && intermediate[1] != '&'*/) {
            intermediate = VarTable::print(intermediate);
        }
        evaluated += "[" + intermediate + "]";
        index++;
    }

    return evaluated;
}

std::vector<std::string> get_var_with_indices(std::string var) {
    int index = 0;
    int size = var.size();
    std::vector<std::string> var_data;
    std::string variable = parser::parse_variable(var, index, false);
    var_data.emplace_back(variable);
    while(index < size) {
        if(var[index] == '[') {
            std::string idx = parser::extract_list(var, '[', index);
            idx = eval(idx.substr(1, idx.size() - 2));
            var_data.emplace_back(idx);
        }
        index++;
    }

    return var_data;
}

std::string expand_var(std::string var) {
    int index = 0;
    std::string variable = parser::parse_variable(var, index, false);
    //int size = var.size();
    if(var[index] == '[') {
        /*index++;
        std::string sub = var.substr(index, size - index - 1);
        sub = eval(sub);
        variable += ("[" + sub + "]");*/
        variable += eval_indices(var, index);
    }
    // get real values here.
    /// dummy call.
    ///
    return variable;
}

bool is_list_or_dict(std::string structure) {
    if(structure[0] == '[' || (structure[0] == '#' && structure[1] == '(')) {
        return true;
    }
    if(/*structure[0] == '$'*/ parser::is_var(structure)/* && structure[1] != '&'*/) {
        Value* temp = VarTable::get(structure, {}, true, true, true);
        if(dynamic_cast<List*>(temp) || dynamic_cast<Dict*>(temp)) {
            return true;
        }
    }
    return false;
}

bool is_num(std::string& data) {
    if(/*data[0] == '$'*/ parser::is_var(data)/* && data[1] != '&'*/) {
        Value* data_temp = VarTable::get(data, {}, true, true, true);
        if(dynamic_cast<Number*>(data_temp)) {
            return true;
        }
    }
    else if(data[0] == '-' || (data[0] >= '0' && data[0] <= '9')) {
        return true;
    }
    return false;
}

bool is_list(std::string& structure) {
    if(structure[0] == '[') {
        return true;
    }
    else if(/*structure[0] == '$'*/ parser::is_var(structure)/* && structure[1] != '&'*/) {
        Value* temp = VarTable::get(structure, {}, true, true, true);
        if(dynamic_cast<List*>(temp)) {
            return true;
        }
    }
    return false;
}

std::deque<std::string> extract_operand(std::deque<std::string>& rpn) {
    std::deque<std::string> expr;
    //rpn.pop_back();
    std::string back = rpn.back();
    if((back[0] >= '0' && back[0] <= '9') || /*back[0] == '$'*/ (parser::is_var(back) /*&& back[1] != '&'*/) || back[0] == '"') {
        expr.push_back(back);
        rpn.pop_back();
    }
    else {
        expr.push_front(back);
        rpn.pop_back();

        std::string second = rpn.back();
        if(!((second[0] >= '0' && second[0] <= '9') || /*second[0] == '$'*/ (parser::is_var(second) /*&& second[1] != '&'*/) || second[0] == '"')) {
            std::deque<std::string> second_expr = extract_operand(rpn);
            while(!second_expr.empty()) {
                expr.push_front(second_expr.back());
                second_expr.pop_back();
            }
        }
        else {
            expr.push_front(second);
            rpn.pop_back();
        }

        if(back != "0n" && back != "!" && back != "~") {
            std::string first = rpn.back();
            if(!((first[0] >= '0' && first[0] <= '9') || /*first[0] == '$'*/ (parser::is_var(second) /*&& second[1] != '&'*/) || first[0] == '"')) {
                std::deque<std::string> first_expr = extract_operand(rpn);
                //print_rpn(first_expr);
                while(!first_expr.empty()) {
                    expr.push_front(first_expr.back());
                    first_expr.pop_back();
                }
            }
            else {
                expr.push_front(first);
                rpn.pop_back();
            }
        }
    }
    return expr;
}


void perform_shortcircuit(std::deque<std::string>& rpn) {
    std::string op = rpn.back();
    rpn.pop_back();
    std::deque<std::string> second = extract_operand(rpn);
    std::deque<std::string> first = extract_operand(rpn);
    std::string first_result = eval(first);
    if((op == "&&" && first_result == "0") || (op == "||" && first_result == "1")) {
        rpn.push_back(first_result);
    }
    else {
        while(!first.empty()) {
            rpn.push_back(first.front());
            first.pop_front();
        }
        while(!second.empty()) {
            rpn.push_back(second.front());
            second.pop_front();
        }
        rpn.push_back(op);
    }
}

std::deque<std::string> make_rpn(std::string& expr, bool shortcircuit = true) {
    //std::cout << "Expr: [" << expr << "]\n";
    std::string current_op = "";
    std::string prev_op = "";
    std::deque<std::string> rpn;
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
            rpn.push_back(value);
            prev_op = value;
            index++;
            continue;
        }
        else if(expr[index] == '$' && expr[index + 1] == '(') {
            std::string value = parser::parse_fexpr(expr, index);
            rpn.push_back(value);
            prev_op = value;
            //index++;
            continue;
        }
        else if(expr[index] == '[') {
            std::string list_value = parser::extract_list(expr, '[', index);
            rpn.push_back(list_value);
            prev_op = list_value;
        }
        else if(expr[index] == '#') {
            index++;
            std::string dict_value = '#' + parser::extract_list(expr, '(', index);
            rpn.push_back(dict_value);
            prev_op = dict_value;
        }
        else if(parser::match(index, expr, "f[", false)) {
            std::string line = parser::extract_fstr(expr, index);
            std::string value = fstr(line);
            rpn.push_back(value);
            prev_op = value;
        }
        else if((expr[index] >= '0' && expr[index] <= '9') || expr[index] == '.') {
            int begin = index;
            while((expr[index] >= '0' && expr[index] <= '9') || expr[index] == '.') {
                index++;
            }
            std::string value = expr.substr(begin, index - begin);
            rpn.push_back(value);
            prev_op = value;
            index--;
        }
        else if(match(expr, parser::null_val, index)) {
            rpn.push_back(parser::null_val);
            prev_op = parser::null_val;
            index++;
            continue;
        }
        else if(match(expr, ops::integer, index)) {
            rpn.push_back(ops::integer);
            prev_op = ops::integer;
            index++;
            continue;
        }
        else if(match(expr, ops::floating, index)) {
            rpn.push_back(ops::floating);
            prev_op = ops::floating;
            index++;
            continue;
        }
        else if(match(expr, ops::string, index)) {
            rpn.push_back(ops::string);
            prev_op = ops::string;
            index++;
            continue;
        }
        else if(/*expr[index] == '$'*/ (parser::is_var(expr, index)/* && expr[index + 1] != '&'*/) && (expr[index] != 'a' || expr[index + 1] != 's')) {
            std::string var = parser::parse_variable(expr, index);
            std::string val = expand_var(var);
            rpn.push_back(val);
            prev_op = val;
            index--;
        }
        else if(match(expr, ops::left, index)) {
            operators.push(ops::left);
            prev_op = ops::left;
        }
        else if(match(expr, ops::right, index)) {
            while(!operators.empty() && operators.top() != ops::left) {
                std::string top_op = operators.top();
                rpn.push_back(top_op);
                if(shortcircuit && (top_op == "&&" || top_op == "||")) {
                    //rpn.pop_back();
                    perform_shortcircuit(rpn); 
                }
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
            SET_CURRENT_OP(ops::walrus);
            SET_CURRENT_OP(ops::t_if);
            SET_CURRENT_OP(ops::t_else);
            SET_CURRENT_OP(ops::as);
            //std::cout << "Prev: " << prev_op << " Current: " << current_op << "\n";
            if(prev_op == "0n" && current_op == "-") {
                index++;
                prev_op = current_op;
                operators.pop();
                continue;
            }
            if(((prev_op == "" || prev_op == ops::left) && current_op == "-") || (current_op == "-" && order(prev_op) != 0)) {
                current_op = "0n";
            }

            prev_op = current_op;

            while(!operators.empty() && operators.top() != ops::left && order(operators.top()) >= order(current_op)) {
                rpn.push_back(operators.top());
                operators.pop();
            }
            operators.push(current_op);
        }

        index++;
    }

    while(!operators.empty()) {
        if(shortcircuit) {
            std::string top_op = operators.top();
            if(top_op == "&&" || top_op == "||") {
                // std::cout << "Top: " << top_op << "\n";
                // ///
                // std::deque<std::string> rpn_copy = rpn;
                // std::cout << "\tRPN: ";
                // while(!rpn_copy.empty()) {
                //     std::cout << rpn_copy.front() << " ";
                //     rpn_copy.pop_front();
                // }
                // std::cout << "\n";
                // ///
                rpn.push_back(top_op);
                perform_shortcircuit(rpn);
                operators.pop();
            }
            else {
                rpn.push_back(operators.top());
                operators.pop();
            }
        }
        else {
            rpn.push_back(operators.top());
            operators.pop();
        }
    }

    return rpn;
}

std::string eval(std::deque<std::string> rpn) {
    /*expr = lib::trim(expr);
    if(expr == "") {
        return expr;
    }*/

    std::string a, b;
    std::string result;
    double x = 0, y = 0;
    std::string token;
    std::stack<std::string> numbers;

    //std::deque<std::string> rpn = make_rpn(expr);

    std::string rpn_front = rpn.front();
    //std::cout << "RPN Front: " << rpn_front << "\n";
    if(rpn.size() == 1 && (rpn_front[0] != '$' || rpn_front[1] != '(')) {
        if(rpn_front[0] >= '0' && rpn_front[0] <= '9') {
            result = lib::trim_num(rpn_front);
            rpn.pop_front();
            return result;
        }
        else if(parser::is_var(rpn_front) /*&& rpn_front[1] != '&'*/) {
            rpn.pop_front();
            return rpn_front;
        }
        rpn.pop_front();
        return rpn_front;
    }

    while(!rpn.empty()) {
        token = rpn.front();
        //std::cout << "Operand: " << token << "\n";
        if(token[0] == '$' && token[1] == '(') {
            std::vector<std::string> function_line = { parser::resolve_fexpr(token) };
            //std::cout << "Resolved Function: " << parser::resolve_fexpr(token) << std::endl;
            std::vector<Token> function_call = lexer::tokenize(function_line);
            //std::cout << "Function Body:\n" << function_call[0].head << " " << function_call[0].values[0] << "\n";
            //std::cout << "Shadow: " << VarTable::get("$n", {}, false, true, true)->shadow.size() << std::endl;
            Value* result = line_exec(function_call, true, true);
            if(result != nullptr) {
                token = result->print();
            }
            else {
                token = "null";
            }
            //std::cout << "Eval Token: " << token << std::endl;
            //std::cout << "Token: " << token << "\n";
            // std::cout << "Ret Val: " << token << "\n";
            delete result;
            //std::cout << function_call[0].head << " " << function_call[0].values[0] << " [Done]\n";
        }
        else if(/*token[0] == '$'*/ (parser::is_var(token) /*&& token[1] != '&'*/) /*&& (token != ops::as && token != ops::integer && token != ops::floating)*/) {
            // avoid getting the print from list and dict types
            Value* temp = VarTable::get(token, {}, true, true, true);
            /// does not eval references in case of walrus operator.
            rpn.pop_front();
            if(rpn.front() == ":=") {
                numbers.push(token);
                continue;
            }
            rpn.push_front(token);
            ///
            if(temp == nullptr) {
                rpn.pop_front();
                numbers.push(token);
                continue;
            }
            else if(rpn.size() >= 3) {
                rpn.pop_front();
                std::string operand_y = rpn.front();
                rpn.pop_front();
                std::string op = rpn.front();
                if(op != ":=") {
                    if(!dynamic_cast<List*>(temp) && !dynamic_cast<Dict*>(temp)) {
                        //std::cout << "Token: " << token << "\n";
                        token = VarTable::print(token);
                    }
                }
                rpn.push_front(operand_y);
                rpn.push_front(token);
            }
            else {
                token = VarTable::print(token);
            }
        }
        rpn.pop_front();
        //std::cout << "Token: [" << token << "] Order: " << order(token) << std::endl;
        if(!order(token)) {
            numbers.push(token);
            continue;
        }
        /*else if(order(token) == 15) {
            std::string ystr = numbers.top();
            numbers.pop();
            if(token == "s") {
                ystr = to_str(ystr);
            }
            else if(token == "f") {
                ystr = to_float(ystr);
            }
            else if(token == "i") {
                ystr = to_integer(ystr);
            }
            numbers.push(ystr);
        }*/

        /*if(token == "||") {
            std::string second = numbers.top();
            numbers.pop();
            std::string first = numbers.top();
            if(first[0] == '$') {
                first = VarTable::print(first);
            }
            numbers.pop();
            if(first == "1") {
                numbers.push(first);
                continue;
            }
            else {
                numbers.push(first);
                if(second[0] == '$') {
                    second = VarTable::print(second);
                }
                numbers.push(second);
            }
        }
        if(token[0] == '$') {
            // avoid getting the print from list and dict types
            std::cout << "Token: " << token << "\n";
            Value* temp = VarTable::get(token, {}, true, true, true);
            //std::cout << "Token Shadow Size: " << temp->shadow.size() << "\n";
            if(!dynamic_cast<List*>(temp) && !dynamic_cast<Dict*>(temp)) {
                //std::cout << "Token: " << token << "\n";
                token = VarTable::print(token);
                //numbers.push(token);
            }
            //token = VarTable::print(token);
        }*/
        else if(order(token) == 14) {
            y = std::stod(numbers.top());
            numbers.pop();
            if(token == "0n") {
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
            b = numbers.top();
            numbers.pop();
            a = numbers.top();
            numbers.pop();
            if(token == ":=") {
                //bool allow_shadowing = VarTable::get(b, {}, true, true, true) != nullptr;
                VarTable::set(a, b, nullptr, VAR, false, depth, true);
                //std::cout << a << " = " << b << "\n";
                numbers.push(b);
                continue;
            }
            else if(token == "==") {
                if(is_list_or_dict(a) || is_list_or_dict(b)) {
                    bool result = compare(a, b);
                    numbers.push(result ? "1" : "0");
                    continue;
                }
            }
            else if(token == "*" && ((is_list(a) && is_num(b)) || (is_num(a) && is_list(b)))) {
                Value* a_temp = nullptr;
                Value* b_temp = nullptr;
                std::string a_val;
                double t_val;
                if(/*a[0] == '$'*/ parser::is_var(a) /*&& a[1] != '&'*/) {
                    a_temp = VarTable::get(a, {}, true, true, true);
                    if(dynamic_cast<List*>(a_temp)) {
                        a_val = VarTable::print(a);
                    }
                }
                else {
                    if(a[0] >= '0' && a[0] <= '9') {
                        t_val = std::stod(a);
                    }
                    else {
                        a_val = a;
                    }
                }
                if(/*b[0] == '$'*/ parser::is_var(b) /*&& b[1] != '&'*/) {
                    b_temp = VarTable::get(b, {}, true, true, true);
                    if(dynamic_cast<Number*>(b_temp)) {
                        t_val = std::stod(dynamic_cast<Number*>(b_temp)->val);
                    }
                    else if(dynamic_cast<List*>(b_temp)) {
                        a_val = VarTable::print(b);
                    }
                }
                else {
                    //t_val = b;
                    if(is_num(b)) {
                        t_val = std::stod(b);
                    }
                    else {
                        a_val = b;
                    }
                }
                numbers.push(list_mul(a_val, t_val));
                continue;
            }

            if(token == "as") {
                if(/*a[0] == '$'*/ parser::is_var(a) /*&& a[1] != '&'*/) {
                    a = VarTable::print(a);
                }
                if(b == "int") {
                    numbers.push(to_integer(a));
                }
                else if(b == "str") {
                    numbers.push(to_str(a));
                }
                else if(b == "float") {
                    numbers.push(to_float(a));
                }
                continue;
            }
            if(token == "?" || token == ":") {
                numbers.push(ternary_op(token, a, b));
                continue;
            }
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
                    errors::invalid_operation(call_stack, line, "strings", token, a, b);
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
                    errors::invalid_operation(call_stack, line, "values", token, a, b);
                }
                continue;
            }

            if(!is_num(a) || !is_num(b)) {
                std::string expr = a + " " + token + " " + b;
                errors::invalid_operation(call_stack, expr, "values", token, a, b);
            }
            x = std::stod(a);
            y = std::stod(b);
            //std::cout << "expr [" << expr << "] x: " << x << " y: " << y << " " << token << "\n";

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

std::string eval(std::string expr) {
    expr = lib::trim(expr);
    if(expr == "") {
        return expr;
    }
    std::deque<std::string> rpn = make_rpn(expr);
    return eval(rpn);
}