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
    std::iota(_orderCheckingConstraintsNewPoint.begin(), _orderCheckingConstraintsNewPoint.end(), 0);
    reorderCheckingCostraints(std::to_string(nextStepInterval.first), std::to_string(nextStepInterval.second));
    return nextStepInterval;
}

long double ModifiedIndexAlgorithm::calculateInterval—haracteristic(long double delta, const std::vector<long double>& marks,
    IndexAlgorithmStepResult previousPointStepResult, IndexAlgorithmStepResult currentPointStepResult) {
    long double intervalCharacteristic;
    if (_modificationMode < ModificationMode::ADAPTIVE_CALCULATE_INTERVAL_CHARACTERISTIC || previousPointStepResult.v == currentPointStepResult.v) {
        intervalCharacteristic = IndexAlgorithm::calculateInterval—haracteristic(delta, marks, previousPointStepResult, currentPointStepResult);
    }
    else {
        auto p = getConstraintOrderNumber(_orderCheckingContraintsByPoint[std::to_string(previousPointStepResult.point)], previousPointStepResult.v);
        auto q = getConstraintOrderNumber(_orderCheckingContraintsByPoint[std::to_string(currentPointStepResult.point)], previousPointStepResult.v);

        auto u = getConstraintOrderNumber(_orderCheckingContraintsByPoint[std::to_string(previousPointStepResult.point)], currentPointStepResult.v);
        auto s = getConstraintOrderNumber(_orderCheckingContraintsByPoint[std::to_string(currentPointStepResult.point)], currentPointStepResult.v);

        if (p < u && q < s) {
            auto v = currentPointStepResult.v;
            auto deltaZ = currentPointStepResult.z - marks[v];

            intervalCharacteristic = 2.0 * delta - 4.0 * deltaZ / (_estimationLipschitzConstant[v] * _params.rCoeff);
        }
        else if (p > u && q > s) {
            auto v = previousPointStepResult.v;
            auto deltaZ = previousPointStepResult.z - marks[v];

            intervalCharacteristic = 2.0 * delta - 4.0 * deltaZ / (_estimationLipschitzConstant[v] * _params.rCoeff);
        }
        else {
            intervalCharacteristic = IndexAlgorithm::calculateInterval—haracteristic(delta, marks, previousPointStepResult, currentPointStepResult);
        }
    }
    return intervalCharacteristic;
}

void ModifiedIndexAlgorithm::reorderCheckingCostraints(const std::string& startIntervalKey, const std::string& endIntervalKey) {

    if (_modificationMode >= ModificationMode::ADAPTIVE_ORDER_CHECKING) {
        auto numberBrokenConstraint = _performedStepsMap[endIntervalKey].v;
        if (numberBrokenConstraint < _taskHelper.getConstraintsCount()) {
            std::rotate(_orderCheckingConstraintsNewPoint.begin(),
                _orderCheckingConstraintsNewPoint.begin() + numberBrokenConstraint,
                _orderCheckingConstraintsNewPoint.begin() + numberBrokenConstraint + 1);
        }
    }

    if (_modificationMode == ModificationMode::IMPROVEMENT_ADAPRIVE_ORDER_CHECKING) {
        auto startIntervalBrokenConstraintOrderNumber = getConstraintOrderNumber(_orderCheckingContraintsByPoint[startIntervalKey], _performedStepsMap[startIntervalKey].v);
        auto endIntervalBrokenConstraintOrderNumber = getConstraintOrderNumber(_orderCheckingContraintsByPoint[endIntervalKey], _performedStepsMap[endIntervalKey].v);

        auto functionIndex = _taskHelper.getConstraintsCount();
        if (startIntervalBrokenConstraintOrderNumber == endIntervalBrokenConstraintOrderNumber && endIntervalBrokenConstraintOrderNumber < functionIndex || startIntervalBrokenConstraintOrderNumber < endIntervalBrokenConstraintOrderNumber && endIntervalBrokenConstraintOrderNumber == functionIndex) { 
            std::rotate(_orderCheckingConstraintsNewPoint.begin(),
                _orderCheckingConstraintsNewPoint.begin() + startIntervalBrokenConstraintOrderNumber,
                _orderCheckingConstraintsNewPoint.begin() + startIntervalBrokenConstraintOrderNumber + 1);
        } else if (startIntervalBrokenConstraintOrderNumber == functionIndex && startIntervalBrokenConstraintOrderNumber > endIntervalBrokenConstraintOrderNumber) {
            std::rotate(_orderCheckingConstraintsNewPoint.begin(),
                _orderCheckingConstraintsNewPoint.begin() + endIntervalBrokenConstraintOrderNumber,
                _orderCheckingConstraintsNewPoint.begin() + endIntervalBrokenConstraintOrderNumber + 1);
        } else if (startIntervalBrokenConstraintOrderNumber < endIntervalBrokenConstraintOrderNumber && endIntervalBrokenConstraintOrderNumber < functionIndex) {
            std::rotate(_orderCheckingConstraintsNewPoint.begin(),
                _orderCheckingConstraintsNewPoint.begin() + endIntervalBrokenConstraintOrderNumber,
                _orderCheckingConstraintsNewPoint.begin() + endIntervalBrokenConstraintOrderNumber + 1);

            std::rotate(_orderCheckingConstraintsNewPoint.begin(),
                _orderCheckingConstraintsNewPoint.begin() + startIntervalBrokenConstraintOrderNumber,
                _orderCheckingConstraintsNewPoint.begin() + startIntervalBrokenConstraintOrderNumber + 1);
        } else if (endIntervalBrokenConstraintOrderNumber < startIntervalBrokenConstraintOrderNumber && startIntervalBrokenConstraintOrderNumber < functionIndex) {
            std::rotate(_orderCheckingConstraintsNewPoint.begin(),
                _orderCheckingConstraintsNewPoint.begin() + startIntervalBrokenConstraintOrderNumber,
                _orderCheckingConstraintsNewPoint.begin() + startIntervalBrokenConstraintOrderNumber + 1);

            std::rotate(_orderCheckingConstraintsNewPoint.begin(),
                _orderCheckingConstraintsNewPoint.begin() + endIntervalBrokenConstraintOrderNumber,
                _orderCheckingConstraintsNewPoint.begin() + endIntervalBrokenConstraintOrderNumber + 1);
        }
    }
}

PointType ModifiedIndexAlgorithm::calculateNextStepPeanoPoint(std::pair<PointType, PointType> nextStepInterval) {
    auto nextStepPeanoPoint = IndexAlgorithm::calculateNextStepPeanoPoint(nextStepInterval);
    _orderCheckingContraintsByPoint[std::to_string(nextStepPeanoPoint)] = _orderCheckingConstraintsNewPoint;
    return nextStepPeanoPoint;
}

std::size_t ModifiedIndexAlgorithm::getConstraintOrderNumber(std::vector<std::size_t> constraintsOrder, std::size_t constraintIndex) {
    auto constraintOrderNumber = utils::indexOf(constraintsOrder, constraintIndex);
    if (!constraintOrderNumber.has_value()) {
        throw errors::MODIF_INDEX_ALGORITHM_INTERNAL_ERROR_ERR_CODE;
    }
    return constraintOrderNumber.value();
}