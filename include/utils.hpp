#pragma once

#include "constants.hpp"
#include "errors.hpp"
#include "operations_helper.hpp"
#include <algorithm>
#include <functional>
#include <math.h>
#include <stack>
#include <string>
#include <vector>


namespace utils {
    void mapd(long double x, int m, long double* y, int n, int key);
    bool isCorrectExpression(const OperationsHelper& operationsHelper, const std::string& infixExpr);
    std::string infixToPostfix(const std::string& infixExpr);
    long double applyUnaryOperation(Operation operation, long double arg);
    long double applyBinaryOperation(Operation operation, long double firstArg, long double secondArg);
}