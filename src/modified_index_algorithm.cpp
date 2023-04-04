#pragma once

#include "modified_index_algorithm.hpp"


ModifiedIndexAlgorithm::ModifiedIndexAlgorithm(const Function& task, const std::vector<Function>& constraints, const IndexAlgorithmParams& params,
    const ModificationMode& orderCheckingMode) : IndexAlgorithm(task, constraints, params), _modificationMode(orderCheckingMode),
    _orderCheckingConstraintsNewPoint(_taskHelper.getConstraintsCount() + 1), _orderCheckingContraintsByPoint() {
    init();
}

ModifiedIndexAlgorithm::ModifiedIndexAlgorithm(IConstrainedOptProblem* generator, const IndexAlgorithmParams& params,
    const ModificationMode& orderCheckingMode) : IndexAlgorithm(generator, params), _modificationMode(orderCheckingMode),
    _orderCheckingConstraintsNewPoint(_taskHelper.getConstraintsCount() + 1), _orderCheckingContraintsByPoint() {
    init();
}

void ModifiedIndexAlgorithm::init() {
    std::vector<std::size_t> orderCheckingContraintsMinPoint(_taskHelper.getConstraintsCount());
    std::vector<std::size_t> orderCheckingContraintsMaxPoint(_taskHelper.getConstraintsCount());

    std::iota(orderCheckingContraintsMinPoint.begin(), orderCheckingContraintsMinPoint.end(), 0);
    std::iota(orderCheckingContraintsMaxPoint.begin(), orderCheckingContraintsMaxPoint.end(), 0);

    _orderCheckingContraintsByPoint[std::to_string(constants::MIN_PEANO_POINT)] = orderCheckingContraintsMinPoint;
    _orderCheckingContraintsByPoint[std::to_string(constants::MAX_PEANO_POINT)] = orderCheckingContraintsMaxPoint;
}

std::string ModifiedIndexAlgorithm::performStep(PointType peanoPoint) {
    Point point = parsePoint(peanoPoint);
    std::string stepKey = std::to_string(peanoPoint);

    std::size_t v = _taskHelper.getConstraintsCount();
    PointType z = -1;
    for (std::size_t i = 0; i < v; i++) {
        z = _taskHelper.getConstraintValue(_orderCheckingContraintsByPoint[stepKey][i], point);
        _complexity.incrementFunctionCalculation(_orderCheckingContraintsByPoint[stepKey][i]);
        if (z > 0) {
            v = _orderCheckingContraintsByPoint[stepKey][i];
            break;
        }
    }

    if (z <= 0) {
        z = _taskHelper.getTaskValue(point);
        _complexity.incrementFunctionCalculation(v);
    }

    maxV = std::max(maxV, v);
    _performedStepsMap[stepKey] = IndexAlgorithmStepResult(peanoPoint, v, z);
    return stepKey;
}

std::pair<PointType, PointType> ModifiedIndexAlgorithm::calculateNextStepInterval(const std::vector<long double>& marks) {
    auto nextStepInterval = IndexAlgorithm::calculateNextStepInterval(marks);
    auto endIntervalKey = std::to_string(nextStepInterval.second);
    std::iota(_orderCheckingConstraintsNewPoint.begin(), _orderCheckingConstraintsNewPoint.end(), 0);
    reorderCheckingCostraints(_performedStepsMap[endIntervalKey].v);
    return nextStepInterval;
}

void ModifiedIndexAlgorithm::reorderCheckingCostraints(std::size_t numberBrokenConstraint) {
    if (_modificationMode >= ModificationMode::ADAPTIVE_ORDER_CHECKING && numberBrokenConstraint < _taskHelper.getConstraintsCount()) {
        std::rotate(_orderCheckingConstraintsNewPoint.begin(),
            _orderCheckingConstraintsNewPoint.begin() + numberBrokenConstraint,
            _orderCheckingConstraintsNewPoint.begin() + numberBrokenConstraint + 1);
    }
}

long double ModifiedIndexAlgorithm::calculateInterval—haracteristic(long double delta, const std::vector<long double>& marks,
    IndexAlgorithmStepResult previousPointStepResult, IndexAlgorithmStepResult currentPointStepResult) {
    long double intervalCharacteristic;
    if (_modificationMode < ModificationMode::ADAPTIVE_CALCULATE_INTERVAL_CHARACTERISTIC || previousPointStepResult.v == currentPointStepResult.v) {
        intervalCharacteristic = IndexAlgorithm::calculateInterval—haracteristic(delta, marks, previousPointStepResult, currentPointStepResult);
    } else {
        auto p = getConstraintOrderNumber(previousPointStepResult.point, previousPointStepResult.v);
        auto q = getConstraintOrderNumber(currentPointStepResult.point, previousPointStepResult.v);

        auto u = getConstraintOrderNumber(previousPointStepResult.point, currentPointStepResult.v);
        auto s = getConstraintOrderNumber(currentPointStepResult.point, currentPointStepResult.v);

        if (p < u && q < s) {
            auto v = currentPointStepResult.v;
            auto deltaZ = currentPointStepResult.z - marks[v];

            intervalCharacteristic = 2.0 * delta - 4.0 * deltaZ / (_estimationLipschitzConstant[v] * _params.rCoeff);
        } else if (p > u && q > s) {
            auto v = previousPointStepResult.v;
            auto deltaZ = previousPointStepResult.z - marks[v];

            intervalCharacteristic = 2.0 * delta - 4.0 * deltaZ / (_estimationLipschitzConstant[v] * _params.rCoeff);
        } else {
            intervalCharacteristic = IndexAlgorithm::calculateInterval—haracteristic(delta, marks, previousPointStepResult, currentPointStepResult);
        }
    }
    return intervalCharacteristic;
}

PointType ModifiedIndexAlgorithm::calculateNextStepPeanoPoint(std::pair<PointType, PointType> nextStepInterval) {
    auto nextStepPeanoPoint = IndexAlgorithm::calculateNextStepPeanoPoint(nextStepInterval);
    _orderCheckingContraintsByPoint[std::to_string(nextStepPeanoPoint)] = _orderCheckingConstraintsNewPoint;
    return nextStepPeanoPoint;
}

std::size_t ModifiedIndexAlgorithm::getConstraintOrderNumber(PointType point, std::size_t constraintIndex) {
    auto pointKey = std::to_string(point);
    auto constraintOrderNumber = utils::indexOf(_orderCheckingContraintsByPoint[pointKey], constraintIndex);
    if (!constraintOrderNumber.has_value()) {
        throw errors::MODIF_INDEX_ALGORITHM_INTERNAL_ERROR_ERR_CODE;
    }
    return constraintOrderNumber.value();
}