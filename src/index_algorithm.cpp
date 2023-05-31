#pragma once

#include "index_algorithm.hpp"


IndexAlgorithm::IndexAlgorithm(const TemplateTask& task, const IndexAlgorithmParams& algParams, const ScanParams& scanParams) :
    _task(task), _algParams(algParams), _scanParams(scanParams),
    _optimalPoint(), _optimalValue(),
    _points(), _complexity(_task.getConstraintsCount() + 1, 2), maxV(),
    _peanoPoints(), _peanoPointsClassification(), _performedStepsMap(),
    _estimationLipschitzConstant(_task.getConstraintsCount() + 1, -DBL_MAX), _minZs(_task.getConstraintsCount() + 1, DBL_MAX) {}

Point IndexAlgorithm::parsePoint(PointType peanoPoint) {
    if (_task.getTaskDimensionSize() > 1) {
        if (peanoPoint < constants::MIN_PEANO_POINT || peanoPoint > constants::MAX_PEANO_POINT) {
            throw errors::INDEX_ALGORITHM_INTERNAL_ERROR_ERR_CODE;
        }

        double* y = new double[_task.getTaskDimensionSize()];
        utils::mapd(peanoPoint, _scanParams.density, y, static_cast<int>(_task.getTaskDimensionSize()), _scanParams.key);

        std::vector<PointType> nonLinearizedPoint(y, y + _task.getTaskDimensionSize());
        auto linearArg = utils::linearTransform(nonLinearizedPoint, _task.getTaskBorders().leftBorder, _task.getTaskBorders().rightBorder);

        delete[] y;

        return linearArg;
    }
    else {
        return { peanoPoint };
    }
}

PointType IndexAlgorithm::startIteration() {
    _peanoPoints.insert(constants::MIN_PEANO_POINT);
    _peanoPoints.insert(constants::MAX_PEANO_POINT);

    Point startIntervalPoint = parsePoint(constants::MIN_PEANO_POINT);
    Point endIntervalPoint = parsePoint(constants::MAX_PEANO_POINT);

    _points.push_back(startIntervalPoint);
    _points.push_back(endIntervalPoint);

    std::string performedStepKey = performStep(constants::MIN_PEANO_POINT);
    updateOptimalPoint(startIntervalPoint, _performedStepsMap[performedStepKey].z,
        _performedStepsMap[performedStepKey].v == _task.getConstraintsCount());
    updateData();

    performedStepKey = performStep(constants::MAX_PEANO_POINT);
    updateOptimalPoint(endIntervalPoint, _performedStepsMap[performedStepKey].z,
        _performedStepsMap[performedStepKey].v == _task.getConstraintsCount());
    updateData();

    auto marks = calculateMarks();
    auto nextStepInterval = calculateNextStepInterval(marks);
    return calculateNextStepPeanoPoint(nextStepInterval);
}

std::string IndexAlgorithm::performStep(PointType peanoPoint) {
    Point point = parsePoint(peanoPoint);
    std::string stepKey = std::to_string(peanoPoint);

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
    _performedStepsMap[stepKey] = IndexAlgorithmStepResult(peanoPoint, v, z);
    PointSetModelHelper::addNewPoint(&_peanoPointsClassification[v], peanoPoint);
    return stepKey;
}

void IndexAlgorithm::updateData() {
    std::string reviewedPointKey;
    for (const auto& [key, pointSetModel] : _peanoPointsClassification) {
        if (PointSetModelHelper::isNeededReview(pointSetModel)) {
            PointType performedStepPeanoPoint = PointSetModelHelper::getNewPoint(pointSetModel);
            PointType performedStepResultZ = _performedStepsMap[std::to_string(performedStepPeanoPoint)].z;

            for (auto reviewedPoint : pointSetModel.reviewedPoints) {
                reviewedPointKey = std::to_string(reviewedPoint);
                _estimationLipschitzConstant[key] = std::max(_estimationLipschitzConstant[key],
                    fabsl(performedStepResultZ - _performedStepsMap[reviewedPointKey].z) /
                    utils::improvementDegree(fabsl(performedStepPeanoPoint - reviewedPoint), 1.0 / _task.getTaskDimensionSize()));
            }

            if (_estimationLipschitzConstant[key] < std::numeric_limits<PointType>::epsilon()) {
                _estimationLipschitzConstant[key] = 1.0;
            }
            _minZs[key] = std::min(_minZs[key], performedStepResultZ);

            PointSetModelHelper::newPointReviewed(&_peanoPointsClassification[key]);
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
    auto previousPointIter = _peanoPoints.begin();
    for (auto currentPointIter = ++_peanoPoints.begin(); currentPointIter != _peanoPoints.end(); currentPointIter++) {
        previousPoint = *previousPointIter;
        currentPoint = *currentPointIter;

        delta = utils::improvementDegree(currentPoint - previousPoint, 1.0 / _task.getTaskDimensionSize());

        R = calculateInterval—haracteristic(delta, marks,
            _performedStepsMap[std::to_string(previousPoint)], _performedStepsMap[std::to_string(currentPoint)]);
        if (R > maxR) {
            maxR = R;
            nextStepInterval = { previousPoint, currentPoint };
        }

        previousPointIter = currentPointIter;
    }

    return nextStepInterval;
}

long double IndexAlgorithm::calculateInterval—haracteristic(long double delta, const std::vector<long double>& marks,
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

PointType IndexAlgorithm::calculateNextStepPeanoPoint(std::pair<PointType, PointType> nextStepInterval) {
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

TrialPoint IndexAlgorithm::run() {
    clearData();

    std::string performedStepKey;
    std::vector<long double> marks;
    std::pair<PointType, PointType> nextStepInterval;
    auto newStepPoint = startIteration();
    Point newPoint = parsePoint(newStepPoint);
    bool isNeededStop = false;
    while (!isNeededStop && _complexity.getIterationCount() < _algParams.iterationLimit) {
        _peanoPoints.insert(newStepPoint);
        performedStepKey = performStep(newStepPoint);
        
        updateOptimalPoint(newPoint, _performedStepsMap[performedStepKey].z,
            _performedStepsMap[performedStepKey].v == _task.getConstraintsCount());
        updateData();
        marks = calculateMarks();

        nextStepInterval = calculateNextStepInterval(marks);
        newStepPoint = calculateNextStepPeanoPoint(nextStepInterval);

        newPoint = parsePoint(newStepPoint);
        _points.push_back(newPoint);
        _complexity.incrementIteration();
        isNeededStop = utils::improvementDegree(nextStepInterval.second - nextStepInterval.first, 1.0 / _task.getTaskDimensionSize()) <= _algParams.accuracy;
    }
    updateOptimalPoint(newPoint, _task.getTaskValue(newPoint));
    _points.push_back(_optimalPoint.value());

    return TrialPoint(_optimalPoint.value(), _optimalValue.value());
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

    if (isValid && (!_optimalValue.has_value() || potentialOptimalValue < _optimalValue)) {
        _optimalPoint = potentialOptimalPoint;
        _optimalValue = potentialOptimalValue;
    }
}

void IndexAlgorithm::clearData() {
    _points.clear();
    _complexity = Complexity(_task.getConstraintsCount() + 1, 2);
    _peanoPoints.clear();
    _peanoPointsClassification.clear();
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