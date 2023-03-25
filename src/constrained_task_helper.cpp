#pragma once

#include "constrained_task_helper.hpp"


ConstrainedTaskHelper::ConstrainedTaskHelper(const Function& task, const std::vector<Function>& constraints) : 
    _task(task), _constraints(constraints) {}

Function ConstrainedTaskHelper::getTask() {
    return _task;
}

Function ConstrainedTaskHelper::getConstraint(std::size_t constraintIndex) {
    if (constraintIndex >= _constraints.size()) {
        throw errors::INDEX_ALGORITHM_INTERNAL_ERROR_ERR_CODE;
    }

    return _constraints[constraintIndex];
}

std::size_t ConstrainedTaskHelper::getConstraintsCount() {
    return _constraints.size();
}

