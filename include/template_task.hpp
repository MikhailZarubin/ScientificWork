#pragma once

#include "IConstrainedOptProblem.hpp"
#include "errors.hpp"
#include "function.hpp"
#include <vector>


class TemplateTask {
    std::optional<Function> _function;
    std::optional<std::vector<Function>> _constraints;
    std::optional<IConstrainedOptProblem*> _constrainedOptProblem;

    TrialPoint _optimumPoint;

public:
    TemplateTask(const Function& task, const std::vector<Function>& constraints, const TrialPoint& optimumPoint);
    TemplateTask(IConstrainedOptProblem* generator, const TrialPoint& optimumPoint);
    TemplateTask(const TemplateTask& templateTask);
    Borders getTaskBorders();
    PointType getTaskValue(Point point);
    PointType getConstraintValue(std::size_t constraintIndex, Point point);
    std::size_t getTaskDimensionSize();
    std::size_t getConstraintsCount();
    Point getOptimumPoint();
    PointType getOptimumValue();
    bool isConstrainedTask();
};