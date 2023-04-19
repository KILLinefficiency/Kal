#include <queue>
#include <deque>
#include <iostream>

double eval(std::string& expr, int& index) {
    double result = 0;
    std::deque<double> numbers;
    std::queue<char> symbols;

    index++;
    while(expr[index - 1] != ')') {
        if(expr[index] == '(') {
            double nested_value = eval(expr, index);
            numbers.push_back(nested_value);
            index++;
            continue;
        }
        if(expr[index] == '+' || expr[index] == '-' || expr[index] == '*' || expr[index] == '/') {
            symbols.push(expr[index]);
        }
        if(expr[index] >= '0' && expr[index] <= '9' || expr[index] == '.') {
            int begin = index;
            while(expr[index] >= '0' && expr[index] <= '9' || expr[index] == '.') {
                index++;
            }
            std::string value = expr.substr(begin, index - begin);
            numbers.push_back(std::stod(value));
        }
        index++;
    }

    while(numbers.size() - 1) {
        double x = numbers.front();
        numbers.pop_front();
        double y = numbers.front();
        numbers.pop_front();
        char op = symbols.front();
        symbols.pop();

        switch(op) {
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

int main() {
    std::string expr = "(1 + (2 * 3 / (2 + (0.5 + 0.5))))";
    expr = "((2 * 2) + (5 * 2))";
    int index = 0;
    double x = eval(expr, index);
    std::cout << expr << " = " << x << std::endl;
    return 0;
}
