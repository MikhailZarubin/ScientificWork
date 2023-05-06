#pragma once

#include "index_algorithm.hpp"


IndexAlgorithm::IndexAlgorithm(const Function& task, const std::vector<Function>& constraints, const IndexAlgorithmParams& params) :
    _taskHelper(task, constraints), _params(params), _points(), _complexity(_taskHelper.getConstraintsCount() + 1, 2), maxV(),
    _peanoPoints(), _peanoPointsClassification(), _performedStepsMap(),
    _estimationLipschitzConstant(_taskHelper.getConstraintsCount() + 1, -DBL_MAX), _minZs(_taskHelper.getConstraintsCount() + 1, DBL_MAX) {}

IndexAlgorithm::IndexAlgorithm(IConstrainedOptProblem* generator, const IndexAlgorithmParams& params) :
    _taskHelper(generator), _params(params), _points(), _complexity(_taskHelper.getConstraintsCount() + 1, 2), maxV(),
    _peanoPoints(), _peanoPointsClassification(), _performedStepsMap(),
    _estimationLipschitzConstant(_taskHelper.getConstraintsCount() + 1, -DBL_MAX), _minZs(_taskHelper.getConstraintsCount() + 1, DBL_MAX) {}

Point IndexAlgorithm::parsePoint(PointType peanoPoint) {
    if (_taskHelper.getTaskDimensionSize() > 1) {
        if (peanoPoint < constants::MIN_PEANO_POINT || peanoPoint > constants::MAX_PEANO_POINT) {
            throw errors::INDEX_ALGORITHM_INTERNAL_ERROR_ERR_CODE;
        }

        double* y = new double[_taskHelper.getTaskDimensionSize()];
        utils::mapd(peanoPoint, constants::DEFAULT_MAPD_M, y, static_cast<int>(_taskHelper.getTaskDimensionSize()), constants::DEFAULT_MAPD_KEY);

        std::vector<PointType> nonLinearizedPoint(y, y + _taskHelper.getTaskDimensionSize());
        auto linearArg = utils::linearTransform(nonLinearizedPoint, _taskHelper.getTaskBorders().leftBorder, _taskHelper.getTaskBorders().rightBorder);

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

    _points.push_back(parsePoint(constants::MIN_PEANO_POINT));
    _points.push_back(parsePoint(constants::MAX_PEANO_POINT));

    std::string performedStepKey = performStep(constants::MIN_PEANO_POINT);
    updateData();

    performedStepKey = performStep(constants::MAX_PEANO_POINT);
    updateData();

    auto marks = calculateMarks();
    auto nextStepInterval = calculateNextStepInterval(marks);
    return calculateNextStepPeanoPoint(nextStepInterval);
}

std::string IndexAlgorithm::performStep(PointType peanoPoint) {
    Point point = parsePoint(peanoPoint);
    std::string stepKey = std::to_string(peanoPoint);

    std::size_t v = _taskHelper.getConstraintsCount();
    PointType z = -1;
    for (std::size_t i = 0; i < v; i++) {
        z = _taskHelper.getConstraintValue(i, point);
        _complexity.incrementFunctionCalculation(i);
        if (z > 0) {
            v = i;
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
                    utils::improvementDegree(fabsl(performedStepPeanoPoint - reviewedPoint), 1.0 / _taskHelper.getTaskDimensionSize()));
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
    std::vector<long double> marks(_taskHelper.getConstraintsCount() + 1);
    for (std::size_t v = 0; v < marks.size(); v++) {
        if (v == maxV) {
            marks[v] = _minZs[v];
            break;
        }
        else {
            marks[v] = -_estimationLipschitzConstant[v] * _params.delta;
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

        delta = utils::improvementDegree(currentPoint - previousPoint, 1.0 / _taskHelper.getTaskDimensionSize());

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
        auto squareCoeff = _params.rCoeff * _params.rCoeff;

        intervalCharacteristic = delta + (deltaZ * deltaZ) / (squareCoeff * squareLipschitzConstant * delta) -
            2.0 * (currentPointStepResult.z + previousPointStepResult.z - 2.0 * marks[v]) /
            (_estimationLipschitzConstant[v] * _params.rCoeff);
    } else if (currentPointStepResult.v > previousPointStepResult.v) {
        auto v = currentPointStepResult.v;
        auto deltaZ = currentPointStepResult.z - marks[v];

        intervalCharacteristic = 2.0 * delta - 4.0 * deltaZ / (_estimationLipschitzConstant[v] * _params.rCoeff);
    } else {
        auto v = previousPointStepResult.v;
        auto deltaZ = previousPointStepResult.z - marks[v];

        intervalCharacteristic = 2.0 * delta - 4.0 * deltaZ / (_estimationLipschitzConstant[v] * _params.rCoeff);
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
            (1.0 / (2.0 * _params.rCoeff)) * utils::improvementDegree(fabsl(_performedStepsMap[endIntervalPointKey].z - _performedStepsMap[startIntervalPointKey].z) /
                _estimationLipschitzConstant[_performedStepsMap[startIntervalPointKey].v], _taskHelper.getTaskDimensionSize());
    }
}

TrialPoint IndexAlgorithm::run() {
    clearData();

    std::optional<Point> optimalPoint;
    std::optional<PointType> optimalValue;
    std::string performedStepKey;
    std::vector<long double> marks;
    std::pair<PointType, PointType> nextStepInterval;
    auto newStepPoint = startIteration();
    bool isNeededStop = false;
    while (!isNeededStop) {
        _peanoPoints.insert(newStepPoint);
        performedStepKey = performStep(newStepPoint);

        if (!optimalValue.has_value() || 
            _performedStepsMap[performedStepKey].v == _taskHelper.getConstraintsCount() &&
            _performedStepsMap[performedStepKey].z < optimalValue.value()) {
            optimalPoint = parsePoint(_performedStepsMap[performedStepKey].point);
            optimalValue = _performedStepsMap[performedStepKey].z;
        }

        updateData();
        marks = calculateMarks();

        nextStepInterval = calculateNextStepInterval(marks);
        newStepPoint = calculateNextStepPeanoPoint(nextStepInterval);

        _points.push_back(parsePoint(newStepPoint));
        _complexity.incrementIteration();
        isNeededStop = utils::improvementDegree(nextStepInterval.second - nextStepInterval.first, 1.0 / _taskHelper.getTaskDimensionSize()) <= _params.accuracy;
    }

    return TrialPoint(optimalPoint.value(), optimalValue.value());
}

void IndexAlgorithm::clearData() {
    _points.clear();
    _complexity = Complexity(_taskHelper.getConstraintsCount() + 1, 2);
    _peanoPoints.clear();
    _peanoPointsClassification.clear();
    _performedStepsMap.clear();
    _estimationLipschitzConstant = std::vector<long double>(_taskHelper.getConstraintsCount() + 1, -DBL_MAX);
    _minZs = std::vector<PointType>(_taskHelper.getConstraintsCount() + 1, DBL_MAX);
}

Points IndexAlgorithm::getPoints() {
    return _points;
}

Complexity IndexAlgorithm::getComplexity() {
    return _complexity;
}