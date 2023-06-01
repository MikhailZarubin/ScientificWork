#pragma once

#include "index_algorithm.hpp"


IndexAlgorithm::IndexAlgorithm(const TemplateTask& task, const IndexAlgorithmParams& algParams, const ScanParams& scanParams) :
    _task(task), _algParams(algParams), _scanParams(scanParams),
    _optimalPoint(), _optimalValue(),
    _points(), _complexity(_task.getConstraintsCount() + 1, 2), maxV(),
    _mappedPoints(), _mappedPointsClassification(), _performedStepsMap(),
    _estimationLipschitzConstant(_task.getConstraintsCount() + 1, -DBL_MAX), _minZs(_task.getConstraintsCount() + 1, DBL_MAX) {}

PointType IndexAlgorithm::startIteration() {
    Point leftIntervalPoint = _task.getTaskDimensionSize() > 1 ?
        utils::getPointFromMapping(_task.getTaskDimensionSize(), _task.getTaskBorders(), _scanParams, constants::MIN_PEANO_POINT) : _task.getTaskBorders().leftBorder;
    Point rigthIntervalPoint = _task.getTaskDimensionSize() > 1 ?
        utils::getPointFromMapping(_task.getTaskDimensionSize(), _task.getTaskBorders(), _scanParams, constants::MAX_PEANO_POINT) : _task.getTaskBorders().rightBorder;

    PointType leftMappedPoint = _task.getTaskDimensionSize() > 1 ?
        constants::MIN_PEANO_POINT : _task.getTaskBorders().leftBorder[0];
    PointType rightMappedPoint = _task.getTaskDimensionSize() > 1 ?
        constants::MAX_PEANO_POINT : _task.getTaskBorders().rightBorder[0];

    _mappedPoints.insert(leftMappedPoint);
    _mappedPoints.insert(rightMappedPoint);

    _points.push_back(leftIntervalPoint);
    _points.push_back(rigthIntervalPoint);

    std::string performedStepKey = performStep(leftMappedPoint);
    updateOptimalPoint(leftIntervalPoint, _performedStepsMap[performedStepKey].z,
        _performedStepsMap[performedStepKey].v == _task.getConstraintsCount());
    updateData();

    performedStepKey = performStep(rightMappedPoint);
    updateOptimalPoint(rigthIntervalPoint, _performedStepsMap[performedStepKey].z,
        _performedStepsMap[performedStepKey].v == _task.getConstraintsCount());
    updateData();

    auto marks = calculateMarks();
    auto nextStepInterval = calculateNextStepInterval(marks);
    return calculateNextStepMappedPoint(nextStepInterval);
}

std::string IndexAlgorithm::performStep(PointType mappedPoint) {
    Point point = utils::getPointFromMapping(_task.getTaskDimensionSize(), _task.getTaskBorders(), _scanParams, mappedPoint);
    std::string stepKey = std::to_string(mappedPoint);

    std::size_t v = _task.getConstraintsCount();
    PointType z = -1;
    for (std::size_t i = 0; i < v; i++) {
        z = _task.getConstraintValue(i, point);
        _complexity.incrementFunctionCalculation(i);
        if (z > 0) {
            v = i;
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

void IndexAlgorithm::updateData() {
    std::string reviewedPointKey;
    for (const auto& [key, pointSetModel] : _mappedPointsClassification) {
        if (PointSetModelHelper::isNeededReview(pointSetModel)) {
            PointType performedStepMappedPoint = PointSetModelHelper::getNewPoint(pointSetModel);
            PointType performedStepResultZ = _performedStepsMap[std::to_string(performedStepMappedPoint)].z;

            for (auto reviewedPoint : pointSetModel.reviewedPoints) {
                reviewedPointKey = std::to_string(reviewedPoint);
                _estimationLipschitzConstant[key] = std::max(_estimationLipschitzConstant[key],
                    fabsl(performedStepResultZ - _performedStepsMap[reviewedPointKey].z) /
                    utils::improvementDegree(fabsl(performedStepMappedPoint - reviewedPoint), 1.0 / _task.getTaskDimensionSize()));
            }

            if (_estimationLipschitzConstant[key] < std::numeric_limits<PointType>::epsilon()) {
                _estimationLipschitzConstant[key] = 1.0;
            }
            _minZs[key] = std::min(_minZs[key], performedStepResultZ);

            PointSetModelHelper::newPointReviewed(&_mappedPointsClassification[key]);
        }
    }
}

std::vector<long double> IndexAlgorithm::calculateMarks() {
    std::vector<long double> marks(_task.getConstraintsCount() + 1);
    for (std::size_t v = 0; v < marks.size(); v++) {
        if (v == maxV) {
            marks[v] = _minZs[v];
            break;
        }
        else {
            marks[v] = -_estimationLipschitzConstant[v] * _algParams.delta;
        }
    }

    return marks;
}

std::pair<PointType, PointType> IndexAlgorithm::calculateNextStepInterval(const std::vector<long double>& marks) {
    std::pair<PointType, PointType> nextStepInterval;
    long double maxR = -DBL_MAX;

    PointType previousPoint, currentPoint;
    long double delta, R;
    auto previousPointIter = _mappedPoints.begin();
    for (auto currentPointIter = ++_mappedPoints.begin(); currentPointIter != _mappedPoints.end(); currentPointIter++) {
        previousPoint = *previousPointIter;
        currentPoint = *currentPointIter;

        delta = utils::improvementDegree(currentPoint - previousPoint, 1.0 / _task.getTaskDimensionSize());

        R = calculateIntervalCharacteristic(delta, marks,
            _performedStepsMap[std::to_string(previousPoint)], _performedStepsMap[std::to_string(currentPoint)]);
        if (R > maxR) {
            maxR = R;
            nextStepInterval = { previousPoint, currentPoint };
        }

        previousPointIter = currentPointIter;
    }

    return nextStepInterval;
}

long double IndexAlgorithm::calculateIntervalCharacteristic(long double delta, const std::vector<long double>& marks,
    IndexAlgorithmStepResult previousPointStepResult, IndexAlgorithmStepResult currentPointStepResult) {
    long double intervalCharacteristic;
    if (previousPointStepResult.v == currentPointStepResult.v) {
        auto v = currentPointStepResult.v;
        auto deltaZ = currentPointStepResult.z - previousPointStepResult.z;
        auto squareLipschitzConstant = _estimationLipschitzConstant[v] * _estimationLipschitzConstant[v];
        auto squareCoeff = _algParams.rCoeff * _algParams.rCoeff;

        intervalCharacteristic = delta + (deltaZ * deltaZ) / (squareCoeff * squareLipschitzConstant * delta) -
            2.0 * (currentPointStepResult.z + previousPointStepResult.z - 2.0 * marks[v]) /
            (_estimationLipschitzConstant[v] * _algParams.rCoeff);
    } else if (currentPointStepResult.v > previousPointStepResult.v) {
        auto v = currentPointStepResult.v;
        auto deltaZ = currentPointStepResult.z - marks[v];

        intervalCharacteristic = 2.0 * delta - 4.0 * deltaZ / (_estimationLipschitzConstant[v] * _algParams.rCoeff);
    } else {
        auto v = previousPointStepResult.v;
        auto deltaZ = previousPointStepResult.z - marks[v];

        intervalCharacteristic = 2.0 * delta - 4.0 * deltaZ / (_estimationLipschitzConstant[v] * _algParams.rCoeff);
    }
    return intervalCharacteristic;
}

PointType IndexAlgorithm::calculateNextStepMappedPoint(std::pair<PointType, PointType> nextStepInterval) {
    std::string startIntervalPointKey = std::to_string(nextStepInterval.first);
    std::string endIntervalPointKey = std::to_string(nextStepInterval.second);
    if (_performedStepsMap[startIntervalPointKey].v != _performedStepsMap[endIntervalPointKey].v) {
        return (nextStepInterval.first + nextStepInterval.second) / 2.0;
    }
    else {
        return (nextStepInterval.first + nextStepInterval.second) / 2.0 -
            utils::sign(_performedStepsMap[endIntervalPointKey].z - _performedStepsMap[startIntervalPointKey].z) *
            (1.0 / (2.0 * _algParams.rCoeff)) * utils::improvementDegree(fabsl(_performedStepsMap[endIntervalPointKey].z - _performedStepsMap[startIntervalPointKey].z) /
                _estimationLipschitzConstant[_performedStepsMap[startIntervalPointKey].v], _task.getTaskDimensionSize());
    }
}

std::optional<TrialPoint> IndexAlgorithm::run() {
    clearData();

    std::string performedStepKey;
    std::vector<long double> marks;
    std::pair<PointType, PointType> nextStepInterval;
    auto newStepMappedPoint = startIteration();
    Point newStepPoint = utils::getPointFromMapping(_task.getTaskDimensionSize(), _task.getTaskBorders(), _scanParams, newStepMappedPoint);
    bool isNeededStop = false;
    while (!isNeededStop && _complexity.getIterationCount() < _algParams.iterationLimit) {
        _mappedPoints.insert(newStepMappedPoint);
        performedStepKey = performStep(newStepMappedPoint);

        updateOptimalPoint(newStepPoint, _performedStepsMap[performedStepKey].z,
            _performedStepsMap[performedStepKey].v == _task.getConstraintsCount());
        updateData();
        marks = calculateMarks();

        nextStepInterval = calculateNextStepInterval(marks);
        newStepMappedPoint = calculateNextStepMappedPoint(nextStepInterval);

        newStepPoint = utils::getPointFromMapping(_task.getTaskDimensionSize(), _task.getTaskBorders(), _scanParams, newStepMappedPoint);
        _points.push_back(newStepPoint);
        _complexity.incrementIteration();
        isNeededStop = utils::improvementDegree(nextStepInterval.second - nextStepInterval.first, 1.0 / _task.getTaskDimensionSize()) <= _algParams.accuracy;
    }
    updateOptimalPoint(newStepPoint, _task.getTaskValue(newStepPoint));
    if (_optimalPoint.has_value()) {
        _points.push_back(_optimalPoint.value());
    }

    return _optimalPoint.has_value() && _optimalValue.has_value() ?
        TrialPoint(_optimalPoint.value(), _optimalValue.value()) : std::optional<TrialPoint>();
}

void IndexAlgorithm::updateOptimalPoint(Point potentialOptimalPoint, PointType potentialOptimalValue,
    std::optional<bool> isValid) {
    if (!isValid.has_value()) {
        isValid = true;
        for (int i = 0; i < _task.getConstraintsCount(); i++) {
            if (_task.getConstraintValue(i, potentialOptimalPoint) > 0) {
                isValid = false;
                break;
            }
        }
    }

    if (isValid.value() && (!_optimalValue.has_value() || potentialOptimalValue < _optimalValue)) {
        _optimalPoint = potentialOptimalPoint;
        _optimalValue = potentialOptimalValue;
    }
}

void IndexAlgorithm::clearData() {
    _points.clear();
    _complexity = Complexity(_task.getConstraintsCount() + 1, 2);
    _mappedPoints.clear();
    _mappedPointsClassification.clear();
    _performedStepsMap.clear();
    _estimationLipschitzConstant = std::vector<long double>(_task.getConstraintsCount() + 1, -DBL_MAX);
    _minZs = std::vector<PointType>(_task.getConstraintsCount() + 1, DBL_MAX);
}

Points IndexAlgorithm::getPoints() {
    return _points;
}

Complexity IndexAlgorithm::getComplexity() {
    return _complexity;
}

TemplateTask IndexAlgorithm::getTask() {
    return _task;
}