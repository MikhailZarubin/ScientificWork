#pragma once

#include "IConstrainedOptProblem.hpp"
#include "errors.hpp"
#include "function.hpp"
#include <vector>


enum class Mode {
    FUNCTION_MODE = 0,
    GENERATOR_MODE = 1
};

class ConstrainedTaskHelper {
    std::optional<Function> _task;
    std::optional<std::vector<Function>> _constraints;
    IConstrainedOptProblem* _generator;
    Mode _mode;

public:
    ConstrainedTaskHelper(const Function& task, const std::vector<Function>& constraints);
    ConstrainedTaskHelper(IConstrainedOptProblem* generator);
    Borders getTaskBorders();
    PointType getTaskValue(Point point);
    PointType getConstraintValue(std::size_t constraintIndex, Point point);
    std::size_t getTaskDimensionSize();
    std::size_t getConstraintsCount();
};