#pragma once

#include <queue>
#include <stack>
#include <iostream>

double eval_expression(std::string expr) {
    double result = 0;
    std::cout << "Expr: [" << expr << "]" << std::endl;
    expr += ' ';

    std::stack<double> numbers;
    std::stack<char> symbols;

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
            numbers.push(std::stod(value));
            std::cout << "[" << std::stod(value) << "]" << std::endl;
        }
        current++;
    }

    while(numbers.size() != 1) {
        double x = numbers.top();
        numbers.pop();
        double y = numbers.top();
        numbers.pop();
        char operator = symbols.top();
        symbols.pop();
        switch(operator) {
            case '+':
                numbers.push(x + y);
                break;
            case '-':
                numbers.push(x - y);
                break;
            case '*':
                numbers.push(x * y);
                break;
            case '/':
                numbers.push(x / y);
        }
    }

    result = numbers.top();
    numbers.pop();

    return result;
}