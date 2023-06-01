#pragma once

#include "function.hpp"


Function::Function(const std::string& expression, const std::string& variableSet, Borders borders) :
    _expression(expression), _variableSet(variableSet), _borders(borders) {}

Function::Function(const Function& function) : 
    _expression(function._expression), _variableSet(function._variableSet), _borders(function._borders) {}

PointType Function::getValue(const Point& point) {
    if (_borders.leftBorder > point || _borders.rightBorder < point) {
        throw ErrorWrapper(Errors::FUNCTION_ERROR, "[FUNCTION] POINT OUT OF RANGE.\n");
    }

    std::string expressionWithValue = _expression;
    std::string::size_type pos = std::string::npos;
    bool isFirstReplace = true;
    for (std::string::size_type i = 0; i < _variableSet.size(); i++) {
        isFirstReplace = true;
        do {
            pos = expressionWithValue.find(_variableSet[i]);
            std::string valuePoint = static_cast<char>(AssistiveSymbols::OPENING_BRACKET) +
                std::to_string(point[static_cast<std::vector<long double>::size_type>(i)]) +
                static_cast<char>(AssistiveSymbols::CLOSING_BRACKET);

            if (pos != std::string::npos) {
                expressionWithValue.replace(pos, 1, valuePoint);
            }
            else if (isFirstReplace) {
                throw ErrorWrapper(Errors::FUNCTION_ERROR, "[FUNCTION] INCORRECT VARIABLE SET.\n");
            }

            isFirstReplace = false;

        } while (pos != std::string::npos);
    }

    return Calculator(expressionWithValue).calculateResult();
}

Point Function::getLeftBorder() {
    return _borders.leftBorder;
}

Point Function::getRightBorder() {
    return _borders.rightBorder;
}

std::size_t Function::getDimensionSize() {
    return _variableSet.size();
}
