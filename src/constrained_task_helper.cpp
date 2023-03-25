#pragma once

#include "constrained_task_helper.hpp"


ConstrainedTaskHelper::ConstrainedTaskHelper(const Function& task, const std::vector<Function>& constraints) : 
    _task(task), _constraints(constraints), _generator(nullptr), _mode(Mode::FUNCTION_MODE) {}

ConstrainedTaskHelper::ConstrainedTaskHelper(IConstrainedOptProblem* generator) :
    _mode(Mode::GENERATOR_MODE) {
    _generator = generator;
}

Borders ConstrainedTaskHelper::getTaskBorders() {
    Point leftBorder, rightBorder;
    switch (_mode) {
    case Mode::FUNCTION_MODE:
        if (!_task.has_value()) {
            throw errors::TASK_HELPER_INTERNAL_ERROR_ERR_CODE;
        }
        leftBorder = _task.value().getLeftBorder();
        rightBorder = _task.value().getRightBorder();
        break;
    case Mode::GENERATOR_MODE:
        if (_generator == nullptr) {
            throw errors::TASK_HELPER_INTERNAL_ERROR_ERR_CODE;
        }
        _generator->GetBounds(leftBorder, rightBorder);
        break;
    default:
        break;
    }
    return Borders(leftBorder, rightBorder);
}

PointType ConstrainedTaskHelper::getTaskValue(Point point) {
    PointType answer;
    switch (_mode) {
    case Mode::FUNCTION_MODE:
        if (!_task.has_value()) {
            throw errors::TASK_HELPER_INTERNAL_ERROR_ERR_CODE;
        }
        answer = _task.value().getValue(point);
        break;
    case Mode::GENERATOR_MODE:
        if (_generator == nullptr) {
            throw errors::TASK_HELPER_INTERNAL_ERROR_ERR_CODE;
        }
        answer = _generator->ComputeFunction(point);
        break;
    default:
        break;
    }
    return answer;
}

PointType ConstrainedTaskHelper::getConstraintValue(std::size_t constraintIndex, Point point) {
    PointType answer;
    switch (_mode) {
    case Mode::FUNCTION_MODE:
        if (!_constraints.has_value() || constraintIndex >= _constraints.value().size()) {
            throw errors::TASK_HELPER_INTERNAL_ERROR_ERR_CODE;
        }
        answer = _constraints.value()[constraintIndex].getValue(point);
        break;
    case Mode::GENERATOR_MODE:
        if (_generator == nullptr|| constraintIndex >= _generator->GetConstraintsNumber()) {
            throw errors::TASK_HELPER_INTERNAL_ERROR_ERR_CODE;
        }
        answer = _generator->ComputeConstraint(constraintIndex, point);
        break;
    default:
        break;
    }
    return answer;
}

std::size_t ConstrainedTaskHelper::getTaskDimensionSize() {
    std::size_t dimensionSize;
    switch (_mode) {
    case Mode::FUNCTION_MODE:
        if (!_task.has_value()) {
            throw errors::TASK_HELPER_INTERNAL_ERROR_ERR_CODE;
        }
        dimensionSize = _task.value().getDimensionSize();
        break;
    case Mode::GENERATOR_MODE:
        if (_generator == nullptr) {
            throw errors::TASK_HELPER_INTERNAL_ERROR_ERR_CODE;
        }
        dimensionSize = _generator->GetDimension();
        break;
    default:
        break;
    }
    return dimensionSize;
}

std::size_t ConstrainedTaskHelper::getConstraintsCount() {
    std::size_t constraintsCount;
    switch (_mode) {
    case Mode::FUNCTION_MODE:
        if (!_constraints.has_value()) {
            throw errors::TASK_HELPER_INTERNAL_ERROR_ERR_CODE;
        }
        constraintsCount = _constraints.value().size();
        break;
    case Mode::GENERATOR_MODE:
        if (_generator == nullptr) {
            throw errors::TASK_HELPER_INTERNAL_ERROR_ERR_CODE;
        }
        constraintsCount = _generator->GetConstraintsNumber();
        break;
    default:
        break;
    }
    return constraintsCount;
}

