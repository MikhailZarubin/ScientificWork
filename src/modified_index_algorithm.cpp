#pragma once

#include "modified_index_algorithm.hpp"


ModifiedIndexAlgorithm::ModifiedIndexAlgorithm(const TemplateTask& task, const IndexAlgorithmParams& algParams, const ScanParams& scanParams) : 
    IndexAlgorithm(task, algParams, scanParams),
    _orderCheckingConstraintsNewPoint(_task.getConstraintsCount() + 1), _orderCheckingContraintsByPoint() {
    std::vector<std::size_t> orderCheckingContraintsMinPoint(_task.getConstraintsCount() + 1);
    std::vector<std::size_t> orderCheckingContraintsMaxPoint(_task.getConstraintsCount() + 1);

    std::iota(orderCheckingContraintsMinPoint.begin(), orderCheckingContraintsMinPoint.end(), 0);
    std::iota(orderCheckingContraintsMaxPoint.begin(), orderCheckingContraintsMaxPoint.end(), 0);

    _orderCheckingContraintsByPoint[std::to_string(_task.getTaskDimensionSize() > 1 ?
        constants::MIN_PEANO_POINT : _task.getTaskBorders().leftBorder[0])] = orderCheckingContraintsMinPoint;
    _orderCheckingContraintsByPoint[std::to_string(_task.getTaskDimensionSize() > 1 ?
        constants::MAX_PEANO_POINT : _task.getTaskBorders().rightBorder[0])] = orderCheckingContraintsMaxPoint;
}

std::string ModifiedIndexAlgorithm::performStep(PointType mappedPoint) {
    Point point = utils::getPointFromMapping(_task.getTaskDimensionSize(), _task.getTaskBorders(), _scanParams, mappedPoint);
    std::string stepKey = std::to_string(mappedPoint);

    std::size_t v = _task.getConstraintsCount();
    PointType z = -1;
    for (std::size_t i = 0; i < v; i++) {
        z = _task.getConstraintValue(_orderCheckingContraintsByPoint[stepKey][i], point);
        _complexity.incrementFunctionCalculation(_orderCheckingContraintsByPoint[stepKey][i]);
        if (z > 0) {
            v = _orderCheckingContraintsByPoint[stepKey][i];
            break;
        }
    }

    if (z <= 0) {
        z = _task.getTaskValue(point);
        _complexity.incrementFunctionCalculation(v);
    }

    maxV = std::max(maxV, v);
    _performedStepsMap[stepKey] = IndexAlgorithmStepResult(mappedPoint, v, z);
    PointSetModelHelper::addNewPoint(&_mappedPointsClassification[v], mappedPoint);
    return stepKey;
}

std::pair<PointType, PointType> ModifiedIndexAlgorithm::calculateNextStepInterval(const std::vector<long double>& marks) {
    auto nextStepInterval = IndexAlgorithm::calculateNextStepInterval(marks);
    std::iota(_orderCheckingConstraintsNewPoint.begin(), _orderCheckingConstraintsNewPoint.end(), 0);
    reorderCheckingCostraints(std::to_string(nextStepInterval.first), std::to_string(nextStepInterval.second));
    return nextStepInterval;
}

long double ModifiedIndexAlgorithm::calculateIntervalCharacteristic(long double delta, const std::vector<long double>& marks,
    IndexAlgorithmStepResult previousPointStepResult, IndexAlgorithmStepResult currentPointStepResult) {
    long double intervalCharacteristic;
    if (previousPointStepResult.v == currentPointStepResult.v) {
        intervalCharacteristic = IndexAlgorithm::calculateIntervalCharacteristic(delta, marks, previousPointStepResult, currentPointStepResult);
    } else {
        auto p = getConstraintOrdinalNumber(_orderCheckingContraintsByPoint[std::to_string(previousPointStepResult.point)], previousPointStepResult.v);
        auto q = getConstraintOrdinalNumber(_orderCheckingContraintsByPoint[std::to_string(currentPointStepResult.point)], previousPointStepResult.v);

        auto u = getConstraintOrdinalNumber(_orderCheckingContraintsByPoint[std::to_string(previousPointStepResult.point)], currentPointStepResult.v);
        auto s = getConstraintOrdinalNumber(_orderCheckingContraintsByPoint[std::to_string(currentPointStepResult.point)], currentPointStepResult.v);

        if (p < u && q < s) {
            auto v = currentPointStepResult.v;
            auto deltaZ = currentPointStepResult.z - marks[v];

            intervalCharacteristic = 2.0 * delta - 4.0 * deltaZ / (_estimationLipschitzConstant[v] * _algParams.rCoeff);
        }
        else if (p > u && q > s) {
            auto v = previousPointStepResult.v;
            auto deltaZ = previousPointStepResult.z - marks[v];

            intervalCharacteristic = 2.0 * delta - 4.0 * deltaZ / (_estimationLipschitzConstant[v] * _algParams.rCoeff);
        }
        else {
            intervalCharacteristic = IndexAlgorithm::calculateIntervalCharacteristic(delta, marks, previousPointStepResult, currentPointStepResult);
        }
    }
    return intervalCharacteristic;
}

void ModifiedIndexAlgorithm::reorderCheckingCostraints(const std::string& startIntervalKey, const std::string& endIntervalKey) {
    auto p = getConstraintOrdinalNumber(_orderCheckingContraintsByPoint[startIntervalKey], _performedStepsMap[startIntervalKey].v);
    auto q = getConstraintOrdinalNumber(_orderCheckingContraintsByPoint[endIntervalKey], _performedStepsMap[endIntervalKey].v);

    auto functionIndex = _task.getConstraintsCount();
    if (p == q && q < functionIndex || p < q && q == functionIndex) {
        std::rotate(_orderCheckingConstraintsNewPoint.begin(),
            _orderCheckingConstraintsNewPoint.begin() + p,
            _orderCheckingConstraintsNewPoint.begin() + p + 1);
    }
    else if (p == functionIndex && p > q) {
        std::rotate(_orderCheckingConstraintsNewPoint.begin(),
            _orderCheckingConstraintsNewPoint.begin() + q,
            _orderCheckingConstraintsNewPoint.begin() + q + 1);
    }
    else if (p < q && q < functionIndex) {
        std::rotate(_orderCheckingConstraintsNewPoint.begin(),
            _orderCheckingConstraintsNewPoint.begin() + q,
            _orderCheckingConstraintsNewPoint.begin() + q + 1);

        std::rotate(_orderCheckingConstraintsNewPoint.begin(),
            _orderCheckingConstraintsNewPoint.begin() + p,
            _orderCheckingConstraintsNewPoint.begin() + p + 1);
    }
    else if (q < p && p < functionIndex) {
        std::rotate(_orderCheckingConstraintsNewPoint.begin(),
            _orderCheckingConstraintsNewPoint.begin() + p,
            _orderCheckingConstraintsNewPoint.begin() + p + 1);

        std::rotate(_orderCheckingConstraintsNewPoint.begin(),
            _orderCheckingConstraintsNewPoint.begin() + q,
            _orderCheckingConstraintsNewPoint.begin() + q + 1);
    }
}

PointType ModifiedIndexAlgorithm::calculateNextStepMappedPoint(std::pair<PointType, PointType> nextStepInterval) {
    auto nextStepMappedPoint = IndexAlgorithm::calculateNextStepMappedPoint(nextStepInterval);
    _orderCheckingContraintsByPoint[std::to_string(nextStepMappedPoint)] = _orderCheckingConstraintsNewPoint;
    return nextStepMappedPoint;
}

std::size_t ModifiedIndexAlgorithm::getConstraintOrdinalNumber(std::vector<std::size_t> constraintsOrder, std::size_t constraintIndex) {
    auto constraintOrderNumber = utils::indexOf(constraintsOrder, constraintIndex);
    if (!constraintOrderNumber.has_value()) {
        throw ErrorWrapper(Errors::MODIFIED_INDEX_ALG_ERROR, "[MODIFIED INDEX ALG] CONSTRAINT WITH INDEX <" + std::to_string(constraintIndex) +
            "> MISSING IN CONSTRAINTS ORDER.\n");;
    }
    return constraintOrderNumber.value();
}