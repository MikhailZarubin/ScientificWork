#pragma once

#include "errors.hpp"
#include "function.hpp"
#include <vector>


class ConstrainedTaskHelper {
    Function _task;
    std::vector<Function> _constraints;

public:
    ConstrainedTaskHelper(const Function& task, const std::vector<Function>& constraints);
    Function getTask();
    Function getConstraint(std::size_t constraintIndex);
    std::size_t getConstraintsCount();
};