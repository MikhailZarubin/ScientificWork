#pragma once

#include "constants.hpp"
#include "errors.hpp"
#include "models.hpp"
#include "operations_helper.hpp"
#include <algorithm>
#include <functional>
#include <math.h>
#include <stack>
#include <string>


namespace utils {
    void mapd(double x, int m, double* y, int n, int key);
    bool isCorrectExpression(const OperationsHelper& operationsHelper, const std::string& infixExpr);
    std::string infixToPostfix(const std::string& infixExpr);
    long double applyUnaryOperation(Operation operation, long double arg);
    long double applyBinaryOperation(Operation operation, long double firstArg, long double secondArg);
    Point linearTransform(const Point nonLinearizedPoint, const Point& leftBorders, const Point& rightBorders);
    PointType sign(PointType arg);
    long double equal(long double arg1, long double arg2);
    long double improvementDegree(long double arg, long double degree);
    PointType getMaxCoordinateDifference(Point point1, Point point2);
    std::vector<std::string> split(const std::string& inputStr, const std::string& delimiter);

    template <class DataType>
    std::optional<DataType> indexOf(std::vector<DataType> collection, DataType elem) {
        auto index = std::distance(collection.begin(), std::find(collection.begin(), collection.end(), elem));
        if (index < collection.size()) {
            return std::optional<DataType>(index);
        }
        else {
            return std::optional<DataType>();
        }
    }

    template <class KeyType, class ValueType>
    bool contains(const std::map<KeyType, ValueType>& inputMap, KeyType key) {
        return inputMap.find(key) != inputMap.end();
    }
}