#pragma once

#include "complexity.hpp"


Complexity::Complexity(std::size_t functionCount, long int iterationCount) :
    _iterationCount(iterationCount), _functionCalculationCount(functionCount, 0) {}

Complexity::Complexity(const Complexity& complexity) {
    _iterationCount = complexity._iterationCount;
    _functionCalculationCount = std::vector<long int>(complexity._functionCalculationCount.begin(), complexity._functionCalculationCount.end());
}

void Complexity::incrementIteration() {
    _iterationCount++;
}

void Complexity::incrementFunctionCalculation(std::size_t funcNumber) {
    if (funcNumber >= _functionCalculationCount.size()) {
        throw errors::COMPLEXITY_INVALID_FUNC_NUMBER_ERROR_ERR_CODE;
    }
    _functionCalculationCount[funcNumber]++;
}

long int Complexity::getIterationCount() {
    return _iterationCount;
}

std::vector<long int> Complexity::getFunctionsCalculationCount() {
    return _functionCalculationCount;
}