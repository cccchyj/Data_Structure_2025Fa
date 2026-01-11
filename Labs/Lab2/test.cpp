#include <iostream>
#include <cassert>
#include <cmath>
#include "RPN.h"  // Assuming you saved the main logic in a separate header or cpp file

using namespace std;

// Function to run a single test case and print results
void testInfixToPostfixAndEvaluate(const string& infix) {
    cout << "Infix Expression: " << infix << endl;
    
    // Convert infix to postfix
    string postfix = infix2postfix(infix);  
    cout << "Postfix Expression: " << postfix << endl;
    
    // Evaluate the postfix expression
    double result = evaluatePostfix(postfix);
    cout << "Evaluated Result: " << result << endl;
    
    cout << "-----------------------------------" << endl;
}

int main() {
    // Test cases
    try {
        // Test 1: Nested parentheses with multiple operations
        testInfixToPostfixAndEvaluate("3 + (2 * (1 + 2))");
        
        // Test 2: Multiple operations with different precedence
        testInfixToPostfixAndEvaluate("5 + 3 * 2 - 8 / 4");
        
        // Test 3: Complex expression with decimals and multiple operators
        testInfixToPostfixAndEvaluate("123.45 * 2.5 + 100 / 4 - 45.67");
        
        // Test 4: Expression with modulus operator
        testInfixToPostfixAndEvaluate("10 % 3 + 4 * 2");

        // Test 5: Large numbers and division
        testInfixToPostfixAndEvaluate("1000000000 / 2500 + 999999");

        // Test 6: Nested operations and mixing of multiplication, division, and addition
        testInfixToPostfixAndEvaluate("(3 + 5 * (10 / 2)) - 4");
        
        // Test 7: Complex algebraic expression with parentheses
        testInfixToPostfixAndEvaluate("(2 + 3) * (5 + 8) / (4 - 1)");
        
        // Test 8: Division with a result that has many decimal places
        testInfixToPostfixAndEvaluate("50 / 3");
        
        // Test 9: Combination of addition, subtraction, multiplication, and division
        testInfixToPostfixAndEvaluate("10 + 2 * 5 - 3 / (7 + 1)");
        
        // Test 10: Division by zero (Edge case)
        cout << "Testing division by zero..." << endl;
        string postfix = infix2postfix("10 / 0");
        if (postfix.empty()) {
            cout << "Handled division by zero correctly!" << endl;
        } else {
            double result = evaluatePostfix(postfix);
            cout << "Evaluated result: " << result << endl;
        }

    } catch (const exception& e) {
        cout << "Test failed with error: " << e.what() << endl;
    }

    return 0;
}
