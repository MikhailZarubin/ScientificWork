#pragma once

#include "modified_index_algorithm.hpp"


ModifiedIndexAlgorithm::ModifiedIndexAlgorithm(const Function& task, const std::vector<Function>& constraints, const IndexAlgorithmParams& params) : IndexAlgorithm(task, constraints, params),
_orderCheckingConstraintsNewPoint(_taskHelper.getConstraintsCount() + 1), _orderCheckingContraintsByPoint() {
    init();
}

ModifiedIndexAlgorithm::ModifiedIndexAlgorithm(IConstrainedOptProblem* generator, const IndexAlgorithmParams& params) : IndexAlgorithm(generator, params),
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
    PointSetModelHelper::addNewPoint(&_peanoPointsClassification[v], peanoPoint);
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
    if (previousPointStepResult.v == currentPointStepResult.v) {
        intervalCharacteristic = IndexAlgorithm::calculateInterval—haracteristic(delta, marks, previousPointStepResult, currentPointStepResult);
    } else {
        auto p = getConstraintOrdinalNumber(_orderCheckingContraintsByPoint[std::to_string(previousPointStepResult.point)], previousPointStepResult.v);
        auto q = getConstraintOrdinalNumber(_orderCheckingContraintsByPoint[std::to_string(currentPointStepResult.point)], previousPointStepResult.v);

        auto u = getConstraintOrdinalNumber(_orderCheckingContraintsByPoint[std::to_string(previousPointStepResult.point)], currentPointStepResult.v);
        auto s = getConstraintOrdinalNumber(_orderCheckingContraintsByPoint[std::to_string(currentPointStepResult.point)], currentPointStepResult.v);

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
    auto p = getConstraintOrdinalNumber(_orderCheckingContraintsByPoint[startIntervalKey], _performedStepsMap[startIntervalKey].v);
    auto q = getConstraintOrdinalNumber(_orderCheckingContraintsByPoint[endIntervalKey], _performedStepsMap[endIntervalKey].v);

    auto functionIndex = _taskHelper.getConstraintsCount();
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

PointType ModifiedIndexAlgorithm::calculateNextStepPeanoPoint(std::pair<PointType, PointType> nextStepInterval) {
    auto nextStepPeanoPoint = IndexAlgorithm::calculateNextStepPeanoPoint(nextStepInterval);
    _orderCheckingContraintsByPoint[std::to_string(nextStepPeanoPoint)] = _orderCheckingConstraintsNewPoint;
    return nextStepPeanoPoint;
}

std::size_t ModifiedIndexAlgorithm::getConstraintOrdinalNumber(std::vector<std::size_t> constraintsOrder, std::size_t constraintIndex) {
    auto constraintOrderNumber = utils::indexOf(constraintsOrder, constraintIndex);
    if (!constraintOrderNumber.has_value()) {
        throw errors::MODIF_INDEX_ALGORITHM_INTERNAL_ERROR_ERR_CODE;
    }
    return constraintOrderNumber.value();
}