#pragma once

#include "global_search_algorithm.hpp"


GlobalSearchAlgorithm::GlobalSearchAlgorithm(const TemplateTask& task, const GlobalSearchAlgorithmParams& algParams, const ScanParams& scanParams) :
    _task(task), _algParams(algParams), _scanParams(scanParams),
    _optimumPoint(), _cachedFunctionValues(), _maxAbsoluteFirstDifference(-DBL_MAX), _algCoefficient(),
    _points(), _complexity(), _checkedPoints() {}

void GlobalSearchAlgorithm::startIteration() {
    Point leftBorder = _task.getTaskDimensionSize() > 1 ?
        utils::getPointFromMapping(_task.getTaskDimensionSize(), _task.getTaskBorders(), _scanParams, constants::MIN_PEANO_POINT) : _task.getTaskBorders().leftBorder;
    Point rightBorder = _task.getTaskDimensionSize() > 1 ?
        utils::getPointFromMapping(_task.getTaskDimensionSize(), _task.getTaskBorders(), _scanParams, constants::MAX_PEANO_POINT) : _task.getTaskBorders().rightBorder;

    PointType leftMappedPoint = _task.getTaskDimensionSize() > 1 ?
        constants::MIN_PEANO_POINT : _task.getTaskBorders().leftBorder[0];
    PointType rightMappedPoint = _task.getTaskDimensionSize() > 1 ?
        constants::MAX_PEANO_POINT : _task.getTaskBorders().rightBorder[0];

    PointType valueLeftBorder = _task.getTaskValue(leftBorder);
    PointType valueRightBorder = _task.getTaskValue(rightBorder);

    if (valueLeftBorder <= valueRightBorder) {
        _optimumPoint = TrialPoint(leftBorder, valueLeftBorder);
    }
    else {
        _optimumPoint = TrialPoint(rightBorder, valueRightBorder);
    }

    _cachedFunctionValues[std::to_string(leftMappedPoint)] = valueLeftBorder;
    _cachedFunctionValues[std::to_string(rightMappedPoint)] = valueRightBorder;

    _checkedPoints.insert(leftMappedPoint);
    _checkedPoints.insert(rightMappedPoint);

    _points.push_back(leftBorder);
    _points.push_back(rightBorder);
}

void GlobalSearchAlgorithm::updateAlgCoefficient(std::set<PointType>::iterator iterNewPoint) {
    std::set<PointType>::iterator start = iterNewPoint, next, end = iterNewPoint;
    if (iterNewPoint != _checkedPoints.begin()) {
        start = std::prev(iterNewPoint);
    }
    if (std::next(iterNewPoint) != _checkedPoints.end()) {
        end = std::next(iterNewPoint);
    }

    while (start != end) {
        next = std::next(start);
        _maxAbsoluteFirstDifference = std::max(_maxAbsoluteFirstDifference,
            fabsl(_cachedFunctionValues[std::to_string(*next)] - _cachedFunctionValues[std::to_string(*start)]) / (*next - *start));
        start = std::next(start);
    }

    if (utils::equal(_maxAbsoluteFirstDifference, 0.0)) {
        _algCoefficient = 1.0;
    }
    else {
        _algCoefficient = _algParams.rCoeff * _maxAbsoluteFirstDifference;
    }
}

std::pair<PointType, PointType> GlobalSearchAlgorithm::calculateNextStepInterval() {
    std::pair<PointType, PointType> nextStepInterval;

    PointType maxIntervalCharacteristic = -DBL_MAX;
    PointType currentIntervalCharacteristic;
    
    PointType previousPoint, currentPoint;
    auto previousPointIter = _checkedPoints.begin();
    for (auto currentPointIter = ++_checkedPoints.begin(); currentPointIter != _checkedPoints.end(); currentPointIter++) {
        previousPoint = *previousPointIter;
        currentPoint = *currentPointIter;

        PointType pointInterval = currentPoint - previousPoint;
        PointType valueInterval = _cachedFunctionValues[std::to_string(currentPoint)] - _cachedFunctionValues[std::to_string(previousPoint)];

        currentIntervalCharacteristic = _algCoefficient * pointInterval +
            (valueInterval * valueInterval) / (_algCoefficient * pointInterval) -
            2.0 * (_cachedFunctionValues[std::to_string(currentPoint)] + _cachedFunctionValues[std::to_string(previousPoint)]);

        if (currentIntervalCharacteristic > maxIntervalCharacteristic) {
            maxIntervalCharacteristic = currentIntervalCharacteristic;
            nextStepInterval = { previousPoint, currentPoint };
        }

        previousPointIter = currentPointIter;
    }
    return nextStepInterval;
}

PointType GlobalSearchAlgorithm::calculateNextStepPoint(std::pair<PointType, PointType> nextStepInterval) {
    return 0.5 * (nextStepInterval.second + nextStepInterval.first) +
        0.5 * (_cachedFunctionValues[std::to_string(nextStepInterval.second)] - _cachedFunctionValues[std::to_string(nextStepInterval.first)]) / _algCoefficient;
}

TrialPoint GlobalSearchAlgorithm::run() {
    clearData();
    startIteration();

    bool stopCondition = false;
    std::set<PointType>::iterator iterNewPoint = _checkedPoints.begin();
    std::pair<PointType, PointType> nextStepInterval;
    PointType nextStepMappedPoint;
    Point nextStepPoint;
    while (!stopCondition) {
        updateAlgCoefficient(iterNewPoint);

        nextStepInterval = calculateNextStepInterval();
        nextStepMappedPoint = calculateNextStepPoint(nextStepInterval);
        nextStepPoint = utils::getPointFromMapping(_task.getTaskDimensionSize(), _task.getTaskBorders(), _scanParams, nextStepMappedPoint);
        _cachedFunctionValues[std::to_string(nextStepMappedPoint)] = _task.getTaskValue(nextStepPoint);

        iterNewPoint = _checkedPoints.insert(nextStepMappedPoint).first;
        _points.push_back(nextStepPoint);

        if (_cachedFunctionValues[std::to_string(nextStepMappedPoint)] < _optimumPoint.value) {
            _optimumPoint = { nextStepPoint, _cachedFunctionValues[std::to_string(nextStepMappedPoint)] };
        }

        stopCondition = utils::improvementDegree(nextStepInterval.second - nextStepInterval.first, 1.0 / _task.getTaskDimensionSize()) <= _algParams.accuracy;
        _complexity.incrementFunctionCalculation();
        _complexity.incrementIteration();
    }
    _points.push_back(_optimumPoint.point);

    return _optimumPoint;
}

Points GlobalSearchAlgorithm::getPoints() {
    return _points;
}

Complexity GlobalSearchAlgorithm::getComplexity() {
    return _complexity;
}

TemplateTask GlobalSearchAlgorithm::getTask() {
    return _task;
}

void GlobalSearchAlgorithm::clearData() {
    _complexity = Complexity();
    _checkedPoints.clear();
    _points.clear();
}
