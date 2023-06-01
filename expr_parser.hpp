#include <sstream>
#include <stack>
#include <iostream>

#define SET_CURRENT_OP(X) else if(match(expr, X, index)) current_op = X

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
    if      (op == ops::negative || op == ops::log_not || op == ops::bit_not)    return 12;
    else if (op == ops::exp)                                                     return 11;
    else if (op == ops::mul || op == ops::div || op == ops::mod)                 return 10;
    else if (op == ops::add || op == ops::sub)                                   return 9;
    else if (op == ops::l_shift || op == ops::r_shift)                           return 8;
    else if (op == ops::lt || op == ops::lte || op == ops::gt || op == ops::gte) return 7;
    else if (op == ops::eq || op == ops::neq)                                    return 6;
    else if (op == ops::bit_and)                                                 return 5;
    else if (op == ops::bit_xor)                                                 return 4;
    else if (op == ops::bit_or)                                                  return 3;
    else if (op == ops::log_and)                                                 return 2;
    else if (op == ops::log_or)                                                  return 1;
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

double eval(std::string expr) {
    double result = 0;
    std::stringstream rpn;
    std::string current_op = "";
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
        else if((expr[index] >= '0' && expr[index] <= '9') || expr[index] == '.') {
            int begin = index;
            while((expr[index] >= '0' && expr[index] <= '9') || expr[index] == '.') {
                index++;
            }
            std::string value = expr.substr(begin, index - begin);
            rpn << value << ' ';
            index--;
        }
        else if(match(expr, ops::left, index)) {
            operators.push(ops::left);
        }
        else if(match(expr, ops::right, index)) {
            while(!operators.empty() && operators.top() != ops::left) {
                rpn << operators.top() << ' ';
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
            SET_CURRENT_OP(ops::negative);

            while(!operators.empty() && operators.top() != ops::left && order(operators.top()) >= order(current_op)) {
                rpn << operators.top() << ' ';
                operators.pop();
            }
            operators.push(current_op);
        }

        index++;
    }

    while(!operators.empty()) {
        rpn << operators.top() << ' ';
        operators.pop();
    }

    double x, y;
    std::string token;
    std::stack<double> numbers;
    while(rpn >> token) {
        if(!order(token)) {
            numbers.push(std::stod(token));
        }
        else if(order(token) == 12) {
            y = numbers.top();
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
            numbers.push(y);
        }
        else {
            y = numbers.top();
            numbers.pop();
            x = numbers.top();
            numbers.pop();
            if     (token == "+")   numbers.push(x + y);
            else if(token == "-")   numbers.push(x - y);
            else if(token == "*")   numbers.push(x * y);
            else if(token == "/")   numbers.push(x / y);
            else if(token == "%")   numbers.push(mod(x, y));
            else if(token == "<<")  numbers.push(long(x) << long(y));
            else if(token == ">>")  numbers.push(long(x) >> long(y));
            else if(token == "<")   numbers.push(x < y);
            else if(token == ">")   numbers.push(x > y);
            else if(token == "<=")  numbers.push(x <= y);
            else if(token == ">=")  numbers.push(x >= y);
            else if(token == "==")  numbers.push(x == y);
            else if(token == "!=")  numbers.push(x != y);
            else if(token == "&")   numbers.push(long(x) & long(y));
            else if(token == "^")   numbers.push(long(x) ^ long(y));
            else if(token == "|")   numbers.push(long(x) | long(y));
            else if(token == "&&")  numbers.push(x && y);
            else if(token == "||")  numbers.push(x || y);
        }
    }
    result = numbers.top();
    numbers.pop();
    return result;
}

/*int main(int argc, char** argv) {
    double res = eval(argv[1]);
    std::cout << res << std::endl;
    return 0; 
}*/
