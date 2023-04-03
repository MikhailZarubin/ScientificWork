#pragma once

#include "modified_index_algorithm.hpp"


ModifiedIndexAlgorithm::ModifiedIndexAlgorithm(const Function& task, const std::vector<Function>& constraints, const IndexAlgorithmParams& params,
    const OrderCheckingMode& orderCheckingMode) : IndexAlgorithm(task, constraints, params), _mode(orderCheckingMode), _orderChecking(_taskHelper.getConstraintsCount()) {
    std::iota(_orderChecking.begin(), _orderChecking.end(), 0);
}

ModifiedIndexAlgorithm::ModifiedIndexAlgorithm(IConstrainedOptProblem* generator, const IndexAlgorithmParams& params,
    const OrderCheckingMode& orderCheckingMode) : IndexAlgorithm(generator, params), _mode(orderCheckingMode), _orderChecking(_taskHelper.getConstraintsCount()) {
    std::iota(_orderChecking.begin(), _orderChecking.end(), 0);
}

std::string ModifiedIndexAlgorithm::performStep(PointType peanoPoint) {
    Point point = parsePoint(peanoPoint);
    std::string stepKey = std::to_string(peanoPoint);

    std::size_t v = _orderChecking.size();
    PointType z = -1;
    for (std::size_t i = 0; i < v; i++) {
        z = _taskHelper.getConstraintValue(_orderChecking[i], point);
        _complexity.incrementFunctionCalculation(_orderChecking[i]);
        if (z > 0) {
            v = _orderChecking[i];
            break;
        }
    }

    if (z <= 0) {
        z = _taskHelper.getTaskValue(point);
        _complexity.incrementFunctionCalculation(v);
    }

    maxV = std::max(maxV, v);
    _performedStepsMap[stepKey] = IndexAlgorithmStepResult(v, z);
    return stepKey;
}

std::pair<PointType, PointType> ModifiedIndexAlgorithm::calculateNextStepInterval(const std::vector<long double>& marks) {
    auto nextStepInterval = IndexAlgorithm::calculateNextStepInterval(marks);
    auto endIntervalKey = std::to_string(nextStepInterval.second);
    std::iota(_orderChecking.begin(), _orderChecking.end(), 0);
    reorderCheckCostraints(_performedStepsMap[endIntervalKey].v);
    return nextStepInterval;
}

void ModifiedIndexAlgorithm::reorderCheckCostraints(std::size_t numberBrokenConstraint) {
    switch (_mode) {
    case OrderCheckingMode::BASE_IMPROVEMENT_MODE:
        if (numberBrokenConstraint > 0 && numberBrokenConstraint < _taskHelper.getConstraintsCount()) {
            std::rotate(_orderChecking.begin(), _orderChecking.begin() + numberBrokenConstraint, _orderChecking.begin() + numberBrokenConstraint + 1);
        }
    default:
        break;
    }
}