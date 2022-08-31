#pragma once

#include <queue>
#include <deque>
#include <iostream>

double eval_expression(std::string expr) {
    double result = 0;
    expr += ' ';

    std::deque<double> numbers;
    std::queue<char> symbols;

    int start = 0;
    int current = 0;
    int len = 0;
    int expr_len = expr.size();
    for(int tok = 0; tok < expr_len; tok++) {
        if(expr[tok] == '+' || expr[tok] == '-' || expr[tok] == '*' || expr[tok] == '/') {
            symbols.push(expr[tok]);
        }
        if((expr[tok] >= '0' && expr[tok] <= '9') || expr[tok] == '.') {
            len++;
        }
        else {
            std::string value = expr.substr(start, len);
            start = current + 1;
            len = 0;
            if(value == "") {
                current++;
                continue;
            }
            numbers.push_back(std::stod(value));
        }
        current++;
    }

    while(numbers.size() != 1) {
        double x = numbers.front();
        numbers.pop_front();
        double y = numbers.front();
        numbers.pop_front();
        char oper = symbols.front();
        symbols.pop();
        switch(oper) {
            case '+':
                numbers.push_front(x + y);
                break;
            case '-':
                numbers.push_front(x - y);
                break;
            case '*':
                numbers.push_front(x * y);
                break;
            case '/':
                numbers.push_front(x / y);
                break;
        }
    }

    result = numbers.front();
    numbers.pop_front();

    return result;
}