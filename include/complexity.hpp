#pragma once

#include "errors.hpp"
#include <vector>


class Complexity {
    long int _iterationCount;
    std::vector<long int> _functionCalculationCount;

public:
    Complexity(std::size_t functionCount = 1, long int iterationCount = 0);
    Complexity(const Complexity& complexity);
    void incrementIteration();
    void incrementFunctionCalculation(std::size_t funcNumber = 0);
    long int getIterationCount();
    std::vector<long int> getFunctionsCalculationCount();
};