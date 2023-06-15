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
    std::vector<DataType> mergeVectors(std::vector<std::vector<DataType>> data, std::size_t startIndex, std::size_t endIndex) {
        std::vector<DataType> result;
        for (std::size_t i = startIndex; i <= std::min(data.size() - 1, endIndex); i++) {
            for (std::size_t j = 0; j < data[i].size(); j++) {
                result.push_back(data[i][j]);
            }
        }
        return result;
    }

    template <class DataType>
    std::vector<DataType> mergeVectors(std::vector<DataType> data1, std::vector<DataType> data2) {
        std::vector<DataType> result(data1);
        for (const auto& elem: data2) {
            result.push_back(elem);
        }
        return result;
    }
}