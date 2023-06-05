#pragma once

#include "utils.hpp"


long double universalPow(long double base, long double deg) {
    if (deg < 0.0) {
        base = 1.0 / base;
        deg = -deg;
    }

    long double checkedCubRoot = deg / (1. / 3.);
    int counter = 0;
    while (checkedCubRoot > 1.0 - std::numeric_limits<long double>::epsilon()) {
        checkedCubRoot -= 1.0;
        counter++;
    }

    if (fabsl(checkedCubRoot) <= std::numeric_limits<long double>::epsilon() && counter % 3) {
        return powl(cbrtl(base), counter);
    }

    return pow(base, deg);
}


bool isOpeningBrackets(const OperationsHelper& operationsHelper, char symbol) {
    return operationsHelper.getPriority(symbol) == Priority::BRACKETS_PRIORITY &&
        symbol != static_cast<char>(AssistiveSymbols::CLOSING_BRACKET);
}
std::string utils::infixToPostfix(const std::string& infixExpr) {
    OperationsHelper operationsHelper;
    std::stack<char> operationsStack;
    std::string postfixExpression;
    char topStack;
    for (char symbol : infixExpr) {
        if (operationsHelper.getPriority(symbol) != Priority::NUMBER_PRIORITY && !postfixExpression.empty() &&
            operationsHelper.getPriority(postfixExpression[postfixExpression.size() - 1]) == Priority::NUMBER_PRIORITY) {
            postfixExpression += static_cast<char>(AssistiveSymbols::DELIMITER);
        }

        if (operationsHelper.getPriority(symbol) == Priority::NUMBER_PRIORITY) {
            postfixExpression += symbol;
        }
        else if (isOpeningBrackets(operationsHelper, symbol)) {
            operationsStack.push(symbol);
        }
        else if (symbol == static_cast<char>(AssistiveSymbols::CLOSING_BRACKET)) {
            topStack = operationsStack.top();
            while (!isOpeningBrackets(operationsHelper, topStack)) {
                postfixExpression += topStack;
                operationsStack.pop();
                topStack = operationsStack.top();
            }
            operationsStack.pop();

            if (topStack != static_cast<char>(AssistiveSymbols::OPENING_BRACKET)) {
                postfixExpression += topStack;
            }
        }
        else {
            topStack = operationsStack.top();
            while (operationsHelper.getPriority(topStack) >= operationsHelper.getPriority(symbol)) {
                postfixExpression += topStack;
                operationsStack.pop();
                topStack = operationsStack.top();
            }
            operationsStack.push(symbol);
        }
    }

    if (!operationsStack.empty()) {
        throw ErrorWrapper(Errors::UTILS_ERROR, "[UTILS] CONVERTING TO POSTFIX OCCURRED ERROR. PERHAPS THERE ARE TOO MANY OPERATIONS IN EXPRESSION.\n");
    }

    return postfixExpression;
}

bool utils::isCorrectExpression(const OperationsHelper& operationsHelper, const std::string& infixExpr) {
    return std::count_if(infixExpr.begin(), infixExpr.end(), [&](char symbol)
        { return operationsHelper.getPriority(symbol) == Priority::BRACKETS_PRIORITY &&
        symbol != static_cast<char>(AssistiveSymbols::CLOSING_BRACKET); }) ==
        std::count_if(infixExpr.begin(), infixExpr.end(), [&](char symbol) {
        return operationsHelper.getPriority(symbol) ==
            Priority::BRACKETS_PRIORITY && symbol == static_cast<char>(AssistiveSymbols::CLOSING_BRACKET); });
}

long double utils::applyBinaryOperation(Operation operation, long double firstArg, long double secondArg) {
    long double resultOperation = DBL_MIN;
    switch (operation) {
    case Operation::SUM:
        resultOperation = secondArg + firstArg;
        break;
    case Operation::SUB:
        resultOperation = secondArg - firstArg;
        break;
    case Operation::MUL:
        resultOperation = secondArg * firstArg;
        break;
    case Operation::DIV:
        resultOperation = secondArg / firstArg;
        break;
    case Operation::POW:
        resultOperation = universalPow(secondArg, firstArg);
        break;
    default:
        break;
    }
    return resultOperation;
}

long double utils::applyUnaryOperation(Operation operation, long double arg) {
    long double resultOperation = DBL_MIN;
    switch (operation) {
    case Operation::SIN:
        resultOperation = sin(arg);
        break;
    case Operation::COS:
        resultOperation = cos(arg);
        break;
    case Operation::LOG:
        resultOperation = log(arg);
        break;
    case Operation::EXP:
        resultOperation = exp(arg);
        break;
    default:
        break;
    }
    return resultOperation;
}

PointType utils::sign(PointType arg) {
    if (fabsl(arg) <= std::numeric_limits<PointType>::epsilon()) {
        return 0.0;
    }
    return arg < 0.0 ? -1.0 : 1.0;
}

long double utils::equal(long double arg1, long double arg2) {
    return std::fabsl(arg1 - arg2) < std::numeric_limits<long double>::epsilon();
}

long double utils::improvementDegree(long double arg, long double degree) {
    bool checkSqrt = utils::equal(degree, 1.0 / 2.0);
    bool checkCbrt = utils::equal(degree, 1.0 / 3.0);
    long double answer;
    if (checkSqrt) {
        answer = sqrtl(arg);
    } else if (checkCbrt) {
        answer = cbrtl(arg);
    } else {
        answer = powl(arg, degree);
    }
    return answer;
}

PointType utils::getMaxCoordinateDifference(Point point1, Point point2) {
    if (point1.size() != point2.size()) {
        throw ErrorWrapper(Errors::UTILS_ERROR, "[UTILS] DIFFERENT SIZES OF POINT 1 AND POINT 2.\n");
    }

    std::size_t dimension = point1.size();
    long double maxDifference = -DBL_MAX;
    for (int coordinate = 0; coordinate < dimension; coordinate++) {
        maxDifference = std::max(fabsl(point1[coordinate] - point2[coordinate]), maxDifference);
    }
    return maxDifference;
}

std::vector<std::string> utils::split(const std::string& inputStr, const std::string& delimiter) {
    std::size_t posStart = 0, posEnd, delimiterLength = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((posEnd = inputStr.find(delimiter, posStart)) != std::string::npos) {
        token = inputStr.substr(posStart, posEnd - posStart);
        posStart = posEnd + delimiterLength;
        res.push_back(token);
    }

    res.push_back(inputStr.substr(posStart));
    return res;
}

Point linearTransform(const Point nonLinearizedPoint, const Point& leftBorders, const Point& rightBorders) {
    Point linearizedPoint(nonLinearizedPoint.size());
    for (std::vector<long double>::size_type i = 0; i < linearizedPoint.size(); i++) {
        linearizedPoint[i] = leftBorders[i] +
            (nonLinearizedPoint[i] + 0.5) * (rightBorders[i] - leftBorders[i]);
    }
    return linearizedPoint;
}

bool utils::stringKeysComparator(const std::string& key1, const std::string& key2) {
    if (key1.size() != key2.size()) {
        return key1.size() < key2.size();
    }
    int keySize = key1.size();
    for (int i = 0; i < keySize; i++) {
        if (key1[i] != key2[i]) {
            return key1[i] < key2[i];
        }
    }
    return key1 < key2;
}

int n1, nexp, l, iq, iu[10], iv[10];
void mapd(double x, int m, double* y, int n, int key) {
    /* mapping y(x) : 1 - center, 2 - line, 3 - node */

    double d, mne, dd, dr;
    float p, r;
    int iw[11];
    int it, is, i, j, k;
    void node(int);

    p = 0.0;
    n1 = n - 1;
    for (nexp = 1, i = 0; i < n; nexp *= 2, i++); 
    d = x;
    r = 0.5;
    it = 0;
    dr = nexp;
    for (mne = 1, i = 0; i < m; mne *= dr, i++);
    for (i = 0; i < n; i++) {
        iw[i] = 1; y[i] = 0.0;
    }
    if (key == 2) {
        d = d * (1.0 - 1.0 / mne); k = 0;
    }
    else
        if (key > 2) {
            dr = mne / nexp;
            dr = dr - fmod(dr, 1.0);
            dd = mne - dr;
            dr = d * dd;
            dd = dr - fmod(dr, 1.0);
            dr = dd + (dd - 1) / (nexp - 1.0);
            dd = dr - fmod(dr, 1.0);
            d = dd * (1. / mne);
        }
    for (j = 0; j < m; j++) {
        iq = 0;
        if (x == 1.0) {
            is = nexp - 1; d = 0.0;
        }
        else {
            d = d * nexp;
            is = d;
            d = d - is;
        }
        i = is;
        node(i);
        i = iu[0];
        iu[0] = iu[it];
        iu[it] = i;
        i = iv[0];
        iv[0] = iv[it];
        iv[it] = i;
        if (l == 0)
            l = it;
        else if (l == it) l = 0;
        if ((iq > 0) || ((iq == 0) && (is == 0)))  k = l;
        else if (iq < 0) k = (it == n1) ? 0 : n1;
        r = r * 0.5;
        it = l;
        for (i = 0; i < n; i++) {
            iu[i] = iu[i] * iw[i];
            iw[i] = -iv[i] * iw[i];
            p = r * iu[i];
            p = p + y[i];
            y[i] = p;
        }
    }
    if (key == 2) {
        if (is == (nexp - 1)) i = -1;
        else i = 1;
        p = 2 * i * iu[k] * r * d;
        p = y[k] - p;
        y[k] = p;
    }
    else if (key == 3) {
        for (i = 0; i < n; i++) {
            p = r * iu[i];
            p = p + y[i];
            y[i] = p;
        }
    }
}

void node(int is) {
    int n, i, j, k1, k2, iff;

    n = n1 + 1;
    if (is == 0) {
        l = n1;
        for (i = 0; i < n; i++) {
            iu[i] = -1; iv[i] = -1;
        }
    }
    else if (is == (nexp - 1)) {
        l = n1;
        iu[0] = 1;
        iv[0] = 1;
        for (i = 1; i < n; i++) {
            iu[i] = -1; iv[i] = -1;
        }
        iv[n1] = 1;
    }
    else {
        iff = nexp;
        k1 = -1;
        for (i = 0; i < n; i++) {
            iff = iff / 2;
            if (is >= iff) {
                if ((is == iff) && (is != 1)) { l = i; iq = -1; }
                is = is - iff;
                k2 = 1;
            }
            else {
                k2 = -1;
                if ((is == (iff - 1)) && (is != 0)) { l = i; iq = 1; }
            }
            j = -k1 * k2;
            iv[i] = j;
            iu[i] = j;
            k1 = k2;
        }
        iv[l] = iv[l] * iq;
        iv[n1] = -iv[n1];
    }
}

Point utils::getPointFromMapping(std::size_t dimensionSize, Borders borders, ScanParams scanParams, PointType mappedPoint) {
    if (dimensionSize > 1) {
        if (mappedPoint < constants::MIN_PEANO_POINT || mappedPoint > constants::MAX_PEANO_POINT) {
            throw ErrorWrapper(Errors::UTILS_ERROR, "[UTILS] RECEIVED MAPPED POINT <" + std::to_string(mappedPoint) +
                ">. EXPECTED MAPPED POINT FROM <" + std::to_string(constants::MIN_PEANO_POINT) + "> TO <" +
                std::to_string(constants::MAX_PEANO_POINT) + ">.\n");
        }

        double* y = new double[dimensionSize];
        mapd(mappedPoint, scanParams.density, y, static_cast<int>(dimensionSize), scanParams.key);

        std::vector<PointType> nonLinearizedPoint(y, y + dimensionSize);
        auto linearArg = linearTransform(nonLinearizedPoint, borders.leftBorder, borders.rightBorder);

        delete[] y;

        return linearArg;
    }
    else {
        return { mappedPoint };
    }
}
