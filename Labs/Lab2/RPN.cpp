#include "RPN.h"


bool isOpening(char c) {
    return c == '(' || c == '[' || c == '{';
}

bool isClosing(char c) {
    return c == ')' || c == ']' || c == '}';
}

bool match(char a, char b) {
    return (a == '(' && b == ')') 
        || (a == '[' && b == ']') 
        || (a == '{' && b == '}');
}

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/' || op == '%') return 2;
    return 0;
}


string infix2postfix(const string& infix) { 
    stack<char> s;
    string postfix = {};
    string operandBuffer = {};

    for (char c : infix) { 
        if (isspace(c)) continue;

        if (isdigit(c) || c == '.') {
            operandBuffer.push_back(c);
        } else {
            if (!operandBuffer.empty()) {
                postfix += operandBuffer + ' ';
                operandBuffer.clear();
            }

            if (isOpening(c)) s.push(c);
            else if (isClosing(c)) {
                while (!s.empty() && !isOpening(s.top())) { 
                    postfix += s.top(); postfix += ' ';
                    s.pop();
                }
                if (!s.empty() && match(s.top(), c)) s.pop();
                else {
                    cerr << "Error: Unbalanced brackets.\n";
                    return "";
                }
            }

            else { // operator
                while (!s.empty() && precedence(c) <= precedence(s.top())) {
                    if (isOpening(s.top())) break;
                    postfix += s.top(); postfix += ' ';
                    s.pop();
                }
                s.push(c);
            }
        }
    }
    if (!operandBuffer.empty())
        postfix += operandBuffer + ' ';
    while (!s.empty()) {
        if (isOpening(s.top())) {
            cerr << "Error: Unbalanced brackets.\n";
            return "";
        }
        postfix += s.top(); postfix += ' ';
        s.pop();
    }
    return postfix;
}

double evaluatePostfix(const string& postfix) {
    stack<double> s;
    stringstream ss(postfix);
    string token;

    while (ss >> token) { 
        if (isdigit(token[0]) || token[0] == '.')
            s.push(stod(token));
        else { // operator
            if (s.size() < 2) {
                cerr << "Error: Invalid expression.\n";
                return 0;
            }
            double b = s.top(); s.pop();
            double a = s.top(); s.pop();
            switch (token[0]) {
                case '+': s.push(a + b); break;
                case '-': s.push(a - b); break;
                case '*': s.push(a * b); break;
                case '/': 
                    if (b == 0) {
                        cerr << "Error: Division by zero.\n";
                        return 0;
                    }
                    s.push(a / b); 
                    break;
                case '%':
                    if (abs(a - round(a)) < 1e-6 && abs(b - round(b)) < 1e-6) {
                        s.push(static_cast<int>(a) % static_cast<int>(b));
                    } else {
                        cerr << "Error: Modulus operation is only valid for integers.";
                        return 0;
                    }
                    break;
                default:
                    cerr << "Error: Invalid operator.\n";
                    return 0;
            }
        }
    }
    return s.empty() ? 0 : s.top();
}