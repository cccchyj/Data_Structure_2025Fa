#pragma once
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <cctype>
#include <cmath>
#include <map>

using std::stack; using std::string; using std::cerr;
using std::stringstream; using std::stod;
using std::isspace; using std::isdigit; using std::round;

bool isOpening(char c);
bool isClosing(char c);
bool match(char a, char b);
int precedence(char op);

string infix2postfix(const string& infix);
double evaluatePostfix(const string& postfix);