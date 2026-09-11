#pragma once

#include <sstream>
#include <stack>
#include <queue>
#include <iostream>
#include <cmath>
#include <sstream>

#include "types.hpp"
#include "globals.hpp"
#include "parser.hpp"
#include "errors.hpp"
#include "types.hpp"
#include "lib/lib_math.hpp"
#include "lib/lib_string.hpp"

std::string eval(std::deque<std::string>, Globals& globals);
bool compare(std::string, std::string, Globals& globals);
Value* line_exec(std::vector<Token>&, bool, bool, bool, Globals&);
Value* make_value(std::string, Globals&);

#define SET_CURRENT_OP(X) else if(match(expr, X, index)) TO_OP(current_op)->op = X

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
    // const std::string negative = "0n",
    //                   log_not = "!",
    //                   bit_not = "~",
    //                   exp = "**",
    //                   mod = "%",
    //                   div = "/",
    //                   mul = "*",
    //                   sub = "-",
    //                   add = "+",
    //                   l_shift = "<<",
    //                   r_shift = ">>",
    //                   lt = "<",
    //                   lte = "<=",
    //                   gt = ">",
    //                   gte = ">=",
    //                   eq = "==",
    //                   neq = "!=",
    //                   bit_and = "&",
    //                   bit_xor = "^",
    //                   bit_or = "|",
    //                   log_and = "&&",
    //                   log_or = "||",
    //                   if_null = "??",
    //                   t_if = "?",
    //                   t_else = ":",
    //                   left = "(",
    //                   right = ")",
    //                   as = "as",
    //                   walrus = ":=",
    //                   integer = "int",
    //                   floating = "float",
    //                   string = "str";
    const Op negative = Op("0n"),
                      log_not = Op("!"),
                      bit_not = Op("~"),
                      exp = Op("**"),
                      mod = Op("%"),
                      div = Op("/"),
                      mul = Op("*"),
                      sub = Op("-"),
                      add = Op("+"),
                      l_shift = Op("<<"),
                      r_shift = Op(">>"),
                      lt = Op("<"),
                      lte = Op("<="),
                      gt = Op(">"),
                      gte = Op(">="),
                      eq = Op("=="),
                      neq = Op("!="),
                      bit_and = Op("&"),
                      bit_xor = Op("^"),
                      bit_or = Op("|"),
                      log_and = Op("&&"),
                      log_or = Op("||"),
                      if_null = Op("??"),
                      t_if = Op("?"),
                      t_else = Op(":"),
                      left = Op("("),
                      right = Op(")"),
                      as = Op("as"),
                      walrus = Op(":="),
                      integer = Op("int"),
                      floating = Op("float"),
                      string = Op("str");
}

bool match(std::string& text, std::string pattern, int& index) {
    int pattern_len = pattern.size();
    bool found = pattern == text.substr(index, pattern_len);
    if(found) {
        index += (pattern_len - 1);
    }
    return found;
}

int order(Value* op) {
    if(TO_OP(op)) {
        std::string OP = TO_OP(op)->op;
        if(OP == ops::as.op)                                                                     return 16;
        else if (OP == ops::if_null.op)                                                          return 15;
        else if (OP == ops::negative.op || OP == ops::log_not.op || OP == ops::bit_not.op)       return 14;
        else if (OP == ops::exp.op)                                                              return 13;
        else if (OP == ops::mul.op || OP == ops::div.op || OP == ops::mod.op)                    return 12;
        else if (OP == ops::add.op || OP == ops::sub.op)                                         return 11;
        else if (OP == ops::l_shift.op || OP == ops::r_shift.op)                                 return 10;
        else if (OP == ops::lt.op || OP == ops::lte.op || OP == ops::gt.op || OP == ops::gte.op) return 9;
        else if (OP == ops::eq.op || OP == ops::neq.op)                                          return 8;
        else if (OP == ops::bit_and.op)                                                          return 7;
        else if (OP == ops::bit_xor.op)                                                          return 6;
        else if (OP == ops::bit_or.op)                                                           return 5;
        else if (OP == ops::log_and.op)                                                          return 4;
        else if (OP == ops::log_or.op)                                                           return 3;
        else if (OP == ops::t_if.op || OP == ops::t_else.op)                                     return 2;
        else if (OP == ops::walrus.op)                                                           return 1;
    }
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

std::string fstr(const std::string& text, Globals& globals) {
    int index = 0;
    std::vector<std::string> values = parser::parse_fstr(text, index);
    int size = values.size();
    if(size == 0) {
        return "";
    }
    values[0] = eval(values[0], globals);
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
            item = eval(values[count + 1], globals);
            if(parser::is_var(item)) {
                item = VarTable::print(item, globals);
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

std::string eval_indices(const std::string& text, int& index, Globals& globals) {
    int size = text.size();
    std::string current, evaluated = "";
    while(index < size) {
        current = parser::extract_list(text, '[', index);
        std::string intermediate = eval(current.substr(1, current.size() - 2), globals);
        if(parser::is_var(intermediate)) {
            intermediate = VarTable::print(intermediate, globals);
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
            idx = eval(idx.substr(1, idx.size() - 2), globals);
            var_data.emplace_back(idx);
        }
        index++;
    }

    return var_data;
}

std::string expand_var(std::string var, Globals& globals) {
    int index = 0;
    std::string variable = parser::parse_variable(var, index, false);
    if(var[index] == '[') {
        variable += eval_indices(var, index, globals);
    }
    return variable;
}

bool is_list_or_dict(std::string structure, Globals& globals) {
    if(structure[0] == '[' || (structure[0] == '#' && structure[1] == '(')) {
        return true;
    }
    if(parser::is_var(structure)) {
        Value* temp = VarTable::get(structure, {}, true, true, true, globals);
        if(dynamic_cast<List*>(temp) || dynamic_cast<Dict*>(temp)) {
            return true;
        }
    }
    return false;
}

bool is_num(std::string& data, Globals& globals) {
    if(parser::is_var(data)) {
        Value* data_temp = VarTable::get(data, {}, true, true, true, globals);
        if(dynamic_cast<Number*>(data_temp)) {
            return true;
        }
    }
    else if(data[0] == '-' || (data[0] >= '0' && data[0] <= '9')) {
        return true;
    }
    return false;
}

bool is_list(std::string& structure, Globals& globals) {
    if(structure[0] == '[') {
        return true;
    }
    else if(parser::is_var(structure)) {
        Value* temp = VarTable::get(structure, {}, true, true, true, globals);
        if(dynamic_cast<List*>(temp)) {
            return true;
        }
    }
    return false;
}

std::deque<const Value*> extract_operand(std::deque<const Value*>& rpn) {
    std::deque<const Value*> expr;
    const Value* back = rpn.back();
    // if((back[0] >= '0' && back[0] <= '9') || (parser::is_var(back)) || back[0] == '"' || (back[0] == '$' && back[1] == '(')) {
    if(TO_NUM(back) || (TO_SYM(back) && parser::is_var(TO_SYM(back)->sym)) || TO_STR(back) || (TO_SYM(back) && TO_SYM(back)->sym[0] == '$' && TO_SYM(back)->sym[1] == '(')) {
        expr.push_back(back);
        rpn.pop_back();
    }
    else {
        expr.push_front(back);
        rpn.pop_back();

        const Value* second = rpn.back();
        // if(!((second[0] >= '0' && second[0] <= '9') || (parser::is_var(second)) || second[0] == '"') || (second[0] == '$' && second[1] == '(')) {
        if(!(TO_NUM(second) || (TO_SYM(second) && parser::is_var(TO_SYM(second)->sym)) || TO_STR(second)) || (TO_SYM(second) && TO_SYM(second)->sym[0] == '$' && TO_SYM(second)->sym[1] == '(')) {
            std::deque<const Value*> second_expr = extract_operand(rpn);
            while(!second_expr.empty()) {
                expr.push_front(second_expr.back());
                second_expr.pop_back();
            }
        }
        else {
            expr.push_front(second);
            rpn.pop_back();
        }

        if(back != &ops::negative && back != &ops::log_not && back != &ops::bit_not) {
            const Value* first = rpn.back();
            // if(!((first[0] >= '0' && first[0] <= '9') || (parser::is_var(second)) || first[0] == '"') || (first[0] == '$' && first[1] == '(')) {
            if(!(TO_NUM(first) || (TO_SYM(second) && parser::is_var(TO_SYM(second)->sym)) || TO_STR(first)) || (TO_SYM(first) && TO_SYM(first)->sym[0] == '$' && TO_SYM(first)->sym[1] == '(')) {
                std::deque<const Value*> first_expr = extract_operand(rpn);
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


void perform_shortcircuit(std::deque<const Value*>& rpn) {
    const Value* op = rpn.back();
    rpn.pop_back();
    std::deque<const Value*> second = extract_operand(rpn);
    std::deque<const Value*> first = extract_operand(rpn);
    std::string first_result = eval(first, globals);
    if((op == &ops::log_and && first_result == "0") || (op == &ops::log_or && first_result == "1")) {
        rpn.push_back(new Number(first_result));
    }
    else {
        rpn.push_back(new Number(first_result));

        while(!second.empty()) {
            rpn.push_back(second.front());
            second.pop_front();
        }
        rpn.push_back(op);
    }
}

std::deque<const Value*> extract_sub_expr(std::deque<const Value*>& rpn) {
    std::deque<const Value*> tokens;

    const Value* op = rpn.back();
    rpn.pop_back();
    tokens.push_front(op);

    int times = 2;
    if(order(const_cast<Value*>(op))) {
        while(times--) {
            if(!order(const_cast<Value*>(rpn.back()))) {
                tokens.push_front(rpn.back());
                rpn.pop_back();
            }
            else {
                std::deque<const Value*> nested_tokens = extract_sub_expr(rpn);
                while(!nested_tokens.empty()) {
                    tokens.push_front(nested_tokens.back());
                    nested_tokens.pop_back();
                }
            }
        }
    }

    return tokens;
}

std::deque<const Value*> extract_ternary(std::deque<const Value*>& rpn, Globals& globals) {
    std::deque<const Value*> ternary;

    const Value* op = rpn.back();
    rpn.pop_back();
    ternary.push_front(op);

    std::deque<const Value*> operand_tokens = extract_sub_expr(rpn);
    while(!operand_tokens.empty()) {
        ternary.push_front(operand_tokens.back());
        operand_tokens.pop_back();
    }

    std::deque<const Value*> condition_tokens = extract_sub_expr(rpn);
    if(condition_tokens.size() == 1) {
        ternary.push_front(condition_tokens.back());
        condition_tokens.pop_back();
    }
    else {
        std::string condition = eval(condition_tokens, globals);
        ternary.push_front(make_value(condition, globals));
    }

    return ternary;
}

void lazy_eval_ternary(std::deque<const Value*>& rpn, Globals& globals) {
    const Value* op = rpn.back();
    rpn.pop_back();
    if(op == &ops::t_if || op == &ops::t_else) {
        std::deque<const Value*> operand_tokens = extract_sub_expr(rpn);

        const Value* condition = rpn.back();
        condition = eval(condition, globals);
        rpn.pop_back();
        if(TO_NUM(condition) && TO_NUM(condition)->val != 0) {
            if(op == &ops::t_else) {
                rpn.push_back(condition);
            }
            else {
                while(!operand_tokens.empty()) {
                    rpn.push_front(operand_tokens.back());
                    operand_tokens.pop_back();
                }
            }
        }
        else {
            rpn.push_back(condition);
            if(op == &ops::t_else) {
                if(TO_NUM(condition) && TO_NUM(condition)->val == 0) {
                    rpn.pop_back();
                }
                while(!operand_tokens.empty()) {
                    rpn.push_front(operand_tokens.back());
                    operand_tokens.pop_back();
                }
            }
        }
    }
}

void lazy_eval_ternary_cascade(std::deque<const Value*>& rpn, Globals& globals) {
    std::deque<const Value*> ternary_tokens = extract_ternary(rpn, globals);
    lazy_eval_ternary(ternary_tokens, globals);
    while(!ternary_tokens.empty()) {
        rpn.push_back(ternary_tokens.front());
        ternary_tokens.pop_front();
    }
}

std::deque<const Value*> make_rpn(std::string& expr, bool shortcircuit, Globals& globals) {
    Value* current_op = new Op();
    Value* prev_op = new Op();
    std::deque<const Value*> rpn;
    std::stack<const Value*> operators;

    int index = 0;
    int expr_len = expr.size();
    while(index < expr_len) {
        if(expr[index] == ops::negative.op[0] || expr[index] == ops::log_not.op[0] || expr[index] == ops::bit_not.op[0]) {
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
            Value* str_value = new String(value);
            rpn.push_back(str_value);
            prev_op = str_value;
            index++;
            continue;
        }
        // TODO
        else if(expr[index] == '$' && expr[index + 1] == '(') {
            std::string value = parser::parse_fexpr(expr, index);
            Value* fexpr_value = new Sym(value);
            rpn.push_back(fexpr_value);
            prev_op = fexpr_value;
            continue;
        }
        else if(expr[index] == '[') {
            std::string value = parser::extract_list(expr, '[', index);
            Value* list_value = new List(value, globals);
            rpn.push_back(list_value);
            prev_op = list_value;
        }
        else if(expr[index] == '#') {
            index++;
            std::string value = '#' + parser::extract_list(expr, '(', index);
            Value* dict_value = new Dict(value, globals);
            rpn.push_back(dict_value);
            prev_op = dict_value;
        }
        else if(parser::match(index, expr, "f(", false)) {
            std::string line = parser::extract_fstr(expr, index);
            std::string value = fstr(line, globals);
            Value* str_value = new String(value);
            rpn.push_back(str_value);
            prev_op = str_value;
        }
        else if((expr[index] >= '0' && expr[index] <= '9') || expr[index] == '.') {
            int begin = index;
            while((expr[index] >= '0' && expr[index] <= '9') || expr[index] == '.') {
                index++;
            }
            std::string value = expr.substr(begin, index - begin);
            Value* num_value = new Number(value);
            rpn.push_back(num_value);
            prev_op = num_value;
            index--;
        }
        else if(match(expr, parser::null_val, index)) {
            Value* null = new Null();
            rpn.push_back(null);
            prev_op = null;
            index++;
            continue;
        }
        else if(match(expr, ops::integer.op, index)) {
            rpn.push_back(&ops::integer);
            prev_op = const_cast<Op*>(&ops::integer);
            index++;
            continue;
        }
        else if(match(expr, ops::floating.op, index)) {
            rpn.push_back(&ops::floating);
            prev_op = const_cast<Op*>(&ops::floating);
            index++;
            continue;
        }
        else if(match(expr, ops::string.op, index)) {
            rpn.push_back(&ops::string);
            prev_op = const_cast<Op*>(&ops::string);
            index++;
            continue;
        }
        // TODO:
        else if((parser::is_var(expr, index)) && (expr[index] != 'a' || expr[index + 1] != 's')) {
            std::string var = parser::parse_variable(expr, index);
            std::string val = expand_var(var, globals);
            Value* value = new Sym(val);
            rpn.push_back(value);
            prev_op = value;
            index--;
        }
        else if(match(expr, ops::left.op, index)) {
            operators.push(&ops::left);
            prev_op = const_cast<Op*>(&ops::left);
        }
        else if(match(expr, ops::right.op, index)) {
            while(!operators.empty() && operators.top() != &ops::left) {
                Value* top_op = const_cast<Value*>(operators.top());
                rpn.push_back(top_op);
                if(shortcircuit && (TO_OP(top_op)->op == "&&" || TO_OP(top_op)->op == "||")) {
                    perform_shortcircuit(rpn); 
                }
                operators.pop();
            }
            operators.pop();
        }

        else {
            if(match(expr, ops::exp.op, index)) current_op = const_cast<Op*>(&ops::exp);
            SET_CURRENT_OP(ops::l_shift.op);
            SET_CURRENT_OP(ops::r_shift.op);
            SET_CURRENT_OP(ops::lte.op);
            SET_CURRENT_OP(ops::gte.op);
            SET_CURRENT_OP(ops::eq.op);
            SET_CURRENT_OP(ops::neq.op);
            SET_CURRENT_OP(ops::log_and.op);
            SET_CURRENT_OP(ops::log_or.op);
            SET_CURRENT_OP(ops::log_not.op);
            SET_CURRENT_OP(ops::bit_not.op);
            SET_CURRENT_OP(ops::mod.op);
            SET_CURRENT_OP(ops::div.op);
            SET_CURRENT_OP(ops::mul.op);
            SET_CURRENT_OP(ops::sub.op);
            SET_CURRENT_OP(ops::add.op);
            SET_CURRENT_OP(ops::lt.op);
            SET_CURRENT_OP(ops::gt.op);
            SET_CURRENT_OP(ops::bit_and.op);
            SET_CURRENT_OP(ops::bit_xor.op);
            SET_CURRENT_OP(ops::bit_or.op);
            SET_CURRENT_OP(ops::if_null.op);
            SET_CURRENT_OP(ops::negative.op);
            SET_CURRENT_OP(ops::walrus.op);
            SET_CURRENT_OP(ops::t_if.op);
            SET_CURRENT_OP(ops::t_else.op);
            SET_CURRENT_OP(ops::as.op);
            if(TO_OP(prev_op)->op == "0n" && TO_OP(current_op)->op == "-") {
                index++;
                prev_op = current_op;
                operators.pop();
                continue;
            }
            if(((TO_OP(prev_op)->op == "" || TO_OP(prev_op)->op == ops::left.op) && TO_OP(current_op)->op == "-") || (TO_OP(current_op)->op == "-" && order(prev_op) != 0)) {
                TO_OP(current_op)->op = "0n";
            }

            TO_OP(prev_op)->op = TO_OP(current_op)->op;

            while(!operators.empty() && operators.top() != &ops::left && order(const_cast<Value*>(operators.top())) >= order(current_op)) {
                if(shortcircuit && (rpn.back() == &ops::t_if || rpn.back() == &ops::t_else)) {
                    lazy_eval_ternary_cascade(rpn, globals);
                }

                rpn.push_back(operators.top());

                if(shortcircuit && (rpn.back() == &ops::t_if || rpn.back() == &ops::t_else)) {
                    lazy_eval_ternary_cascade(rpn, globals);
                }
                operators.pop();
            }
            operators.push(current_op);
        }

        index++;
    }

    while(!operators.empty()) {
        if(shortcircuit) {
            Value* top_op = const_cast<Value*>(operators.top());
            if(TO_OP(top_op) && top_op == &ops::if_null) {
                std::deque<const Value*> fallback = extract_sub_expr(rpn);
                std::deque<const Value*> original = extract_sub_expr(rpn);
                std::string result = eval(original, globals);

                if(result != "null") {
                    rpn.push_back(make_value(result, globals));
                }
                else {
                    while(!fallback.empty()) {
                        rpn.push_back(fallback.front());
                        fallback.pop_front();
                    }
                }

                operators.pop();
            }
            else if(TO_OP(top_op) && (top_op == &ops::log_and || top_op == &ops::log_or)) {
                rpn.push_back(top_op);
                perform_shortcircuit(rpn);
                operators.pop();
            }
            else if(TO_OP(top_op) && (top_op == &ops::t_if || top_op == &ops::t_else)) {
                rpn.push_back(top_op);
                lazy_eval_ternary_cascade(rpn, globals);
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

std::string eval(std::deque<const Value*> rpn, Globals& globals) {
    int& depth = globals.depth;

    std::string a, b;
    std::string result;
    double x = 0, y = 0;
    // std::string token;
    Value* token;
    std::stack<const Value*> numbers;

    Value* rpn_front = rpn.front();
    if(rpn.size() == 1 && (TO_SYM(rpn_front)->sym[0] != '$' || TO_SYM(rpn_front)->sym[1] != '(')) {
        if(order(rpn_front)) {
            // ERR:
            errors::invalid_operator(rpn_front);
        }
        if(TO_NUM(rpn_front)/*rpn_front[0] >= '0' && rpn_front[0] <= '9'*/) {
            // result = lib::trim_num(rpn_front);
            result = rpn.front();
            rpn.pop_front();
            return result;
        }
        else if(TO_SYM(rpn_front) && parser::is_var(TO_SYM(rpn_front)->sym)) {
            rpn.pop_front();
            return rpn_front;
        }
        else if(TO_STR(rpn_front)/*rpn_front[0] == '"'*/) {
            rpn_front = lib::render_escape_chars(rpn_front);
        }
        rpn.pop_front();
        return rpn_front;
    }

    while(!rpn.empty()) {
        token = rpn.front();
        if(TO_SYM(token) && TO_SYM(token)->sym[0] == '$' && TO_SYM(token)->sym[1] == '(') {
            std::vector<std::string> function_line = { parser::resolve_fexpr(token) };
            std::vector<Token> function_call = lexer::tokenize(function_line, globals);
            Value* result = line_exec(function_call, true, true, false, globals);
            if(result != nullptr) {
                //token = result->print();
                token = copy(result);
            }
            else {
                token = new Null();
            }
            delete result;
        }
        else if(TO_SYM(token) && parser::is_var(TO_SYM(token)->sym)) {
            Value* temp = VarTable::get(TO_SYM(token)->sym, {}, true, true, true, globals);
            rpn.pop_front();
            //if(rpn.front() == &ops::walrus) {
            if(rpn.front() == &ops::walrus) {
                numbers.push(token);
                continue;
            }
            rpn.push_front(token);
            if(temp == nullptr) {
                rpn.pop_front();
                numbers.push(token);
                continue;
            }
            else if(rpn.size() >= 3) {
                rpn.pop_front();
                const Value* operand_y = rpn.front();
                rpn.pop_front();
                const Value* op = rpn.front();
                if(op != &ops::walrus) {
                    if(!dynamic_cast<List*>(temp) && !dynamic_cast<Dict*>(temp)) {
                        token = VarTable::print(token, globals);
                    }
                }
                rpn.push_front(operand_y);
                rpn.push_front(token);
            }
            else {
                token = VarTable::print(token, globals);
            }
        }
        rpn.pop_front();
        if(!order(token)) {
            numbers.push(token);
            continue;
        }
        else if(order(token) == 14) {
            if(TO_NUM(numbers.top)) {
                y = TO_NUM(numbers.top())->val;
                numbers.pop();
                //if(token == &ops::negative) {
                if(token == &ops::negative) {
                    y = -y;
                }
                else if(token == &ops::log_not) {
                    y = !y;
                }
                else if(token == &ops::bit_not) {
                    y = ~long(y);
                }
                numbers.push(new Number(y));
            }
        }
        else {
            if(numbers.size() < 2) {
                errors::missing_operands(numbers.top());
            }
            b = numbers.top();
            numbers.pop();
            a = numbers.top();
            numbers.pop();
            if(token == ":=") {
                VarTable::set(a, b, nullptr, VAR, false, depth, true, globals);
                numbers.push(b);
                continue;
            }
            else if(token == "==") {
                if(is_list_or_dict(a, globals) || is_list_or_dict(b, globals)) {
                    bool result = compare(a, b, globals);
                    numbers.push(result ? "1" : "0");
                    continue;
                }
            }
            else if(token == "*" && ((is_list(a, globals) && is_num(b, globals)) || (is_num(a, globals) && is_list(b, globals)))) {
                Value* a_temp = nullptr;
                Value* b_temp = nullptr;
                std::string a_val;
                double t_val;
                if(parser::is_var(a)) {
                    a_temp = VarTable::get(a, {}, true, true, true, globals);
                    if(dynamic_cast<List*>(a_temp)) {
                        a_val = VarTable::print(a, globals);
                    }
                }
                else {
                    if(a[0] >= '0' && a[0] <= '9') {
                        t_val = std::stod(a);
                        if(t_val < 0) {
                            errors::neg_list_mul(globals, t_val);
                        }
                    }
                    else {
                        a_val = a;
                    }
                }
                if(parser::is_var(b)) {
                    b_temp = VarTable::get(b, {}, true, true, true, globals);
                    if(dynamic_cast<Number*>(b_temp)) {
                        t_val = dynamic_cast<Number*>(b_temp)->val;
                    }
                    else if(dynamic_cast<List*>(b_temp)) {
                        a_val = VarTable::print(b, globals);
                    }
                }
                else {
                    if(is_num(b, globals)) {
                        t_val = std::stod(b);
                        if(t_val < 0) {
                            errors::neg_list_mul(globals, t_val);
                        }
                    }
                    else {
                        a_val = b;
                    }
                }
                numbers.push(list_mul(a_val, t_val));
                continue;
            }

            if(token == "as") {
                if(parser::is_var(a)) {
                    a = VarTable::print(a, globals);
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
                    errors::invalid_operation(globals, "strings", token, a, b);
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
                    errors::invalid_operation(globals, "values", token, a, b);
                }
                continue;
            }

            if(!is_num(a, globals) || !is_num(b, globals)) {
                std::string expr = a + " " + token + " " + b;
                errors::invalid_operation(globals, "values", token, a, b);
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

    // if(numbers.size() != 1) {
    //     // ERR:
    //     errors::invalid_expression(numbers.top());
    // }

    result = lib::trim_num(numbers.top());
    if(result[0] == '"') {
        result = lib::render_escape_chars(result);
    }
    numbers.pop();
    return result;
}

// std::string eval(std::string expr, Globals& globals) {
//     expr = lib::trim(expr);
//     if(expr == "") {
//         return expr;
//     }
//     std::deque<Value*> rpn = make_rpn(expr, true, globals);
//     return eval(rpn, globals);
// }