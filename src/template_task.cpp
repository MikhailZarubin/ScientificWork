#pragma once

#include "template_task.hpp"


TemplateTask::TemplateTask(const Function& function, const std::vector<Function>& constraints, const TrialPoint& optimumPoint) :
    _function(function), _constraints(constraints), _constrainedOptProblem(), _optimumPoint(optimumPoint) {}

TemplateTask::TemplateTask(IConstrainedOptProblem* constrainedOptProblem, const TrialPoint& optimumPoint) :
    _function(), _constraints(), _constrainedOptProblem(constrainedOptProblem), _optimumPoint(optimumPoint) {}

TemplateTask::TemplateTask(const TemplateTask& templateTask) :
    _function(templateTask._function), _constraints(templateTask._constraints),
    _constrainedOptProblem(templateTask._constrainedOptProblem), _optimumPoint(templateTask._optimumPoint) {}

Borders TemplateTask::getTaskBorders() {
    Point leftBorder, rightBorder;
    if (_function.has_value()) {
        leftBorder = _function.value().getLeftBorder();
        rightBorder = _function.value().getRightBorder();
    }
    else if (_constrainedOptProblem.has_value() && _constrainedOptProblem.value() != nullptr) {
        _constrainedOptProblem.value()->GetBounds(leftBorder, rightBorder);
    }
    else {
        throw ErrorWrapper(Errors::TEMPLATE_TASK_ERROR, "[TEMPLATE TASK] TASK IS NOT INITIALIZED.\n");
    }
    return Borders(leftBorder, rightBorder);
}

PointType TemplateTask::getTaskValue(Point point) {
    PointType answer;
    if (_function.has_value()) {
        answer = _function.value().getValue(point);
    }
    else if (_constrainedOptProblem.has_value() && _constrainedOptProblem.value() != nullptr) {
        answer = _constrainedOptProblem.value()->ComputeFunction(point);
    }
    else {
        throw ErrorWrapper(Errors::TEMPLATE_TASK_ERROR, "[TEMPLATE TASK] TASK IS NOT INITIALIZED.\n");
    }
    return answer;
}

PointType TemplateTask::getConstraintValue(std::size_t constraintIndex, Point point) {
    PointType answer;
    if (_constraints.has_value()) {
        if (constraintIndex < _constraints.value().size()) {
            answer = _constraints.value()[constraintIndex].getValue(point);
        }
        else {
            throw ErrorWrapper(Errors::TEMPLATE_TASK_ERROR, "[TEMPLATE TASK] RECEIVED CONSTRAINT INDEX OUT OF RANGE.\n");
        }
    }
    else if (_constrainedOptProblem.has_value() && _constrainedOptProblem.value() != nullptr) {
        if (constraintIndex < _constrainedOptProblem.value()->GetConstraintsNumber()) {
            answer = _constrainedOptProblem.value()->ComputeConstraint(constraintIndex, point);
        }
        else {
            throw ErrorWrapper(Errors::TEMPLATE_TASK_ERROR, "[TEMPLATE TASK] RECEIVED CONSTRAINT INDEX OUT OF RANGE.\n");
        }
    }
    else {
        throw ErrorWrapper(Errors::TEMPLATE_TASK_ERROR, "[TEMPLATE TASK] TASK IS NOT INITIALIZED.\n");
    }
    return answer;
}

std::size_t TemplateTask::getTaskDimensionSize() {
    std::size_t dimensionSize;
    if (_function.has_value()) {
        dimensionSize = _function.value().getDimensionSize();
    }
    else if (_constrainedOptProblem.has_value() && _constrainedOptProblem.value() != nullptr) {
        dimensionSize = _constrainedOptProblem.value()->GetDimension();
    }
    else {
        throw ErrorWrapper(Errors::TEMPLATE_TASK_ERROR, "[TEMPLATE TASK] TASK IS NOT INITIALIZED.\n");
    }
    return dimensionSize;
}

std::size_t TemplateTask::getConstraintsCount() {
    std::size_t constraintsCount;
    if (_constraints.has_value()) {
        constraintsCount = _constraints.value().size();
    }
    else if (_constrainedOptProblem.has_value() && _constrainedOptProblem.value() != nullptr) {
        constraintsCount = _constrainedOptProblem.value()->GetConstraintsNumber();
    }
    else {
        throw ErrorWrapper(Errors::TEMPLATE_TASK_ERROR, "[TEMPLATE TASK] TASK IS NOT INITIALIZED.\n");
    }
    return constraintsCount;
}

Point TemplateTask::getOptimumPoint() {
    return _optimumPoint.point;
}

PointType TemplateTask::getOptimumValue() {
    return _optimumPoint.value;
}

