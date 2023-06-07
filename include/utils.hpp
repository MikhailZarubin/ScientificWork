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
    bool isCorrectExpression(const OperationsHelper& operationsHelper, const std::string& infixExpr);
    std::string infixToPostfix(const std::string& infixExpr);
    long double applyUnaryOperation(Operation operation, long double arg);
    long double applyBinaryOperation(Operation operation, long double firstArg, long double secondArg);
    PointType sign(PointType arg);
    long double equal(long double arg1, long double arg2);
    long double improvementDegree(long double arg, long double degree);
    PointType getMaxCoordinateDifference(Point point1, Point point2);
    std::vector<std::string> split(const std::string& inputStr, const std::string& delimiter);
    Point getPointFromMapping(std::size_t dimensionSize, Borders borders, ScanParams scanParams, PointType mappedPoint);
    bool stringKeysComparator(const std::string& key1, const std::string& key2);
    long findSumAllElements(const std::vector<long>& data);

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

    template <class DataType>
    std::string convertToString(const std::vector<DataType>& data, const std::string& delimiter, std::function<std::string(const DataType&)> converter) {
        std::string convertedData;
        auto iter = data.begin();

        while (std::next(iter) != data.end()) {
            convertedData += converter(*iter) + delimiter;
            iter = std::next(iter);
        }
        convertedData += converter(*iter);

        return convertedData;
    }
}