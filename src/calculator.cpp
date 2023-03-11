#pragma once

#include "calculator.hpp"

namespace Priority {
    int DELIMITER_PRIORITY = -1; // |
    int NULL_PRIORITY = 0; // number
    int LOW_PRIORITY = 1; // brackets and beginnings of complex functions
    int MEDIUM_PRIORITY = 2; // + and -
    int HIGH_PRIORITY = 3; // * and /
    int HIGHEST_PRIORITY = 4; // ^
}

namespace PowAssistant {
    long double universalPow(long double value, long double exp) {
        long double error = 0.001;

        if (exp < 0.0) {
            value = 1.0 / value;
            exp = -exp;
        }

        long double checkedCubRoot = exp / (1. / 3.);

        int counter = 0;
        while (checkedCubRoot > 1.0 - error) {
            checkedCubRoot -= 1.0;
            counter++;
        }

        if (fabsl(checkedCubRoot) <= error && counter % 3) {
            return powl(cbrtl(value), counter);
        }

        return pow(value, exp);

    }
}


int Calculator::getPriority(char symbol) const
{
    switch (symbol)
    {
    case'+': return Priority::MEDIUM_PRIORITY;
    case'-': return Priority::MEDIUM_PRIORITY;
    case'*': return Priority::HIGH_PRIORITY;
    case'/': return Priority::HIGH_PRIORITY;
    case'(':return Priority::LOW_PRIORITY;
    case')':return Priority::LOW_PRIORITY;
    case'^':return Priority::HIGHEST_PRIORITY;
    case's':return Priority::LOW_PRIORITY;
    case'c':return Priority::LOW_PRIORITY;
    case'l':return Priority::LOW_PRIORITY;
    case'e':return Priority::LOW_PRIORITY;
    case '|': return Priority::DELIMITER_PRIORITY;
    default: return Priority::NULL_PRIORITY;
    }
}


bool Calculator::beginComplexFunction(char symbol) const {
    return symbol == 's' || symbol == 'c' || symbol == 'e' || symbol == 'l';
}


bool Calculator::openingBrackets(char symbol) const {
    return symbol == '(' || beginComplexFunction(symbol);
}


Calculator::Calculator(const std::string& basicExpression) : infixExpression{}, postfixExpression{} {
    if (!configureExpression(basicExpression)) {
        throw 0;
    }
}


bool Calculator::configureExpression(const std::string& basicExpression) {
    infixExpression = "(";

    for (std::string::size_type i = 0; i < basicExpression.size(); i++)
    {
        if (basicExpression[i] == 'e' && basicExpression[i + 1] == '-') {
            infixExpression += 'F';
            i++;
        }
        else if (beginComplexFunction(basicExpression[i])) {
            infixExpression += basicExpression[i];
            i += 3;

            if (i >= basicExpression.size() - 2)
                throw 0;
        }

        else if (basicExpression[i] != ' '){
            std::string::size_type size = infixExpression.size();

            if ((size == 0 || (size > 0 && openingBrackets(infixExpression[size - 1]))) && basicExpression[i] == '-')
                infixExpression += '0';

            infixExpression += basicExpression[i];
        }
    }

    infixExpression += ")";

    return checkCorrectnessExpression();
}


bool Calculator::checkCorrectnessExpression() const {
    int countBrackets{ 0 };

    for (const auto& symbol : infixExpression)
    {
        if (openingBrackets(symbol))
            countBrackets++;

        else if (symbol == ')')
        {
            if (countBrackets == 0)
                return false;

            countBrackets--;
        }
    }
    return countBrackets == 0;
}


void Calculator::infixToPostfix() {
    std::stack<char> operationsStack;
    char topStack;

    postfixExpression.clear();

    for (const auto& symbol : infixExpression)
    {
        if (getPriority(symbol) != Priority::NULL_PRIORITY && !postfixExpression.empty() && getPriority(postfixExpression[postfixExpression.size() - 1]) == Priority::NULL_PRIORITY)
            postfixExpression += '|';

        if (getPriority(symbol) == Priority::NULL_PRIORITY)
            postfixExpression += symbol;

        else if (openingBrackets(symbol))
            operationsStack.push(symbol);

        else if (symbol == ')')
        {
            topStack = operationsStack.top();

            while (!openingBrackets(topStack))
            {
                postfixExpression += topStack;

                operationsStack.pop();
                topStack = operationsStack.top();
            }

            operationsStack.pop();

            if (topStack != '(')
                postfixExpression += topStack;
        }

        else
        {
            topStack = operationsStack.top();

            while (getPriority(topStack) >= getPriority(symbol))
            {
                postfixExpression += topStack;

                operationsStack.pop();
                topStack = operationsStack.top();
            }

            operationsStack.push(symbol);
        }
    }

    if (!operationsStack.empty())
        throw 0;
}


long double Calculator::calculateResult() {
    infixToPostfix();

    std::stack<PointType> valuesStack;

    long double a, b;
    std::string number;

    for (std::string::size_type i = 0; i < postfixExpression.size(); i++)
    {
        switch (postfixExpression[i])
        {

        case'+':
            a = valuesStack.top(); valuesStack.pop();
            b = valuesStack.top();
            valuesStack.top() = b + a;
            break;

        case'-':
            a = valuesStack.top(); valuesStack.pop();
            b = valuesStack.top();
            valuesStack.top() = b - a;
            break;

        case'*':
            a = valuesStack.top(); valuesStack.pop();
            b = valuesStack.top();
            valuesStack.top() = b * a;
            break;

        case'/':
            a = valuesStack.top(); valuesStack.pop();
            b = valuesStack.top();
            valuesStack.top() = b / a;
            break;

        case'^':
            a = valuesStack.top(); valuesStack.pop();
            b = valuesStack.top();
            valuesStack.top() = PowAssistant::universalPow(b, a);
            break;

        case's':
            a = valuesStack.top();
            valuesStack.top() = sin(a);
            break;

        case'c':
            a = valuesStack.top();
            valuesStack.top() = cos(a);
            break;

        case'e':
            a = valuesStack.top();
            valuesStack.top() = exp(a);
            break;

        case'l':
            a = valuesStack.top();
            valuesStack.top() = log(a);
            break;

        default:

            while (i < postfixExpression.size() && getPriority(postfixExpression[i]) == 0 && postfixExpression[i] != '|')
            {
                if (postfixExpression[i] == 'F')
                    number += "e-";
                else
                    number += postfixExpression[i];
                i++;
            }

            valuesStack.push(std::stold(number));
            number.clear();

            break;
        }
    }

    if (valuesStack.size() != 1)
        throw 0;

    return valuesStack.top();
}