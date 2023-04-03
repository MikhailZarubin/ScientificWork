#pragma once

#include "index_algorithm.hpp"


IndexAlgorithm::IndexAlgorithm(const Function& task, const std::vector<Function>& constraints, const IndexAlgorithmParams& params) :
    _taskHelper(task, constraints), _params(params), _points(), _complexity(_taskHelper.getConstraintsCount() + 1), maxV(),
    _peanoPoints(), _peanoPointsClassification(), _performedStepsMap(),
    _estimationLipschitzConstant(_taskHelper.getConstraintsCount() + 1, -DBL_MAX), _minZs(_taskHelper.getConstraintsCount() + 1, DBL_MAX) {}

IndexAlgorithm::IndexAlgorithm(IConstrainedOptProblem* generator, const IndexAlgorithmParams& params) :
    _taskHelper(generator), _params(params), _points(), _complexity(_taskHelper.getConstraintsCount() + 1), maxV(),
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

    std::string performedStepKey = performStep(constants::MIN_PEANO_POINT);
    updateData(performedStepKey, constants::MIN_PEANO_POINT);

    performedStepKey = performStep(constants::MAX_PEANO_POINT);
    updateData(performedStepKey, constants::MAX_PEANO_POINT);

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
    _performedStepsMap[stepKey] = IndexAlgorithmStepResult(v, z);
    return stepKey;
}

void IndexAlgorithm::updateData(const std::string& performedStepKey, PointType performedStepPeanoPoint) {
    IndexAlgorithmStepResult performedStepResult = _performedStepsMap[performedStepKey];

    std::string stepKey;
    for (auto peanoPoint : _peanoPointsClassification[performedStepResult.v]) {
        stepKey = std::to_string(peanoPoint);
        _estimationLipschitzConstant[performedStepResult.v] = std::max(_estimationLipschitzConstant[performedStepResult.v],
            fabs(performedStepResult.z - _performedStepsMap[stepKey].z) /
            powl(fabs(performedStepPeanoPoint - peanoPoint), 1.0 / _taskHelper.getTaskDimensionSize()));
    }

    if (_estimationLipschitzConstant[performedStepResult.v] <= 0.0 || _peanoPointsClassification[performedStepResult.v].empty()) {
        _estimationLipschitzConstant[performedStepResult.v] = 1.0;
    }

    _peanoPointsClassification[performedStepResult.v].insert(performedStepPeanoPoint);
    _minZs[performedStepResult.v] = std::min(_minZs[performedStepResult.v], performedStepResult.z);
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
    IndexAlgorithmStepResult previousPointStepResult, currentPointStepResult;
    long double delta, R;
    auto previousPointIter = _peanoPoints.begin();
    for (auto currentPointIter = ++_peanoPoints.begin(); currentPointIter != _peanoPoints.end(); currentPointIter++) {
        previousPoint = *previousPointIter;
        currentPoint = *currentPointIter;

        previousPointStepResult = _performedStepsMap[std::to_string(previousPoint)];
        currentPointStepResult = _performedStepsMap[std::to_string(currentPoint)];

        delta = utils::improvementDegree(currentPoint - previousPoint, 1.0 / _taskHelper.getTaskDimensionSize());

        if (previousPointStepResult.v == currentPointStepResult.v) {

            R = delta + ((currentPointStepResult.z - previousPointStepResult.z) * (currentPointStepResult.z - previousPointStepResult.z)) /
                (_params.rCoeff * _params.rCoeff * _estimationLipschitzConstant[currentPointStepResult.v] * _estimationLipschitzConstant[currentPointStepResult.v] * delta) -
                2.0 * (currentPointStepResult.z + previousPointStepResult.z - 2.0 * marks[currentPointStepResult.v]) /
                (_estimationLipschitzConstant[currentPointStepResult.v] * _params.rCoeff);
        }
        else if (currentPointStepResult.v > previousPointStepResult.v) {
            R = 2.0 * delta - 4.0 * (currentPointStepResult.z - marks[currentPointStepResult.v]) /
                (_estimationLipschitzConstant[currentPointStepResult.v] * _params.rCoeff);
        }
        else {
            R = 2.0 * delta - 4.0 * (previousPointStepResult.z - marks[previousPointStepResult.v]) /
                (_estimationLipschitzConstant[previousPointStepResult.v] * _params.rCoeff);
        }

        if (R > maxR) {
            maxR = R;
            nextStepInterval = { previousPoint, currentPoint };
        }

        previousPointIter = currentPointIter;
    }

    return nextStepInterval;
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

    std::string performedStepKey;
    std::vector<long double> marks;
    std::pair<PointType, PointType> nextStepInterval;
    auto newStepPoint = startIteration();
    bool isNeededStop = false;
    while (!isNeededStop) {
        _peanoPoints.insert(newStepPoint);
        performedStepKey = performStep(newStepPoint);
        updateData(performedStepKey, newStepPoint);
        marks = calculateMarks();

        nextStepInterval = calculateNextStepInterval(marks);
        newStepPoint = calculateNextStepPeanoPoint(nextStepInterval);

        _points.push_back(parsePoint(newStepPoint));
        _complexity.incrementIteration();
        isNeededStop = utils::improvementDegree(nextStepInterval.second - nextStepInterval.first, 1.0 / _taskHelper.getTaskDimensionSize()) <= _params.accuracy;
    }

    auto arguments = parsePoint(newStepPoint);
    _points.push_back(arguments);
    auto value = _taskHelper.getTaskValue(arguments);

    return TrialPoint(arguments, value);
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