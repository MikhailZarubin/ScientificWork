#pragma once

#include "global_search_algorithm.hpp"

namespace {
    PointType convertPointToBidimensional(Point point) {
        if (point.size() != constants::BIDIMENSIONAL_POINT_SIZE) {
            throw errors::INCORRECT_POINT_SIZE_ERR_CODE;
        }
        return point[constants::BIDIMENSIONAL_POINT_SIZE - 1];
    }

    bool comparisonPair(const TrialPoint& firstPoint, const TrialPoint& secondPoint) {
        return convertPointToBidimensional(firstPoint.point) < convertPointToBidimensional(secondPoint.point);
    }
}


BidimensionalGlobalSearch::BidimensionalGlobalSearch(const Function& task, const GlobalSearchTaskParams& params) :
    _task(task), _params(params), _globalMinimum(), _points(), _complexity(), _checkedPoints() {}

void BidimensionalGlobalSearch::startIteration() {
    Point leftBorder = _task.getLeftBorder();
    Point rightBorder = _task.getRightBorder();
    PointType valueFuncLeftBorder = _task.getValue(leftBorder);
    PointType valueFuncRightBorder = _task.getValue(rightBorder);

    if (valueFuncLeftBorder <= valueFuncRightBorder)
        _globalMinimum = TrialPoint(leftBorder, valueFuncLeftBorder);
    else
        _globalMinimum = TrialPoint(rightBorder, valueFuncRightBorder);

    _checkedPoints.insert({ leftBorder, valueFuncLeftBorder });
    _checkedPoints.insert({ rightBorder, valueFuncRightBorder });

    _points.push_back(leftBorder);
    _points.push_back(rightBorder);
}

TrialPoint BidimensionalGlobalSearch::run() {
    clearCache();
    startIteration();

    bool stopÑondition = false;
    PointType maxDiff = PointType();

    while (!stopÑondition)
    {
        for (auto iter = ++_checkedPoints.begin(); iter != _checkedPoints.end(); iter++)
        {
            TrialPoint currentElem = *(iter--);
            TrialPoint lastELem = *(iter++);

            PointType diff = abs((currentElem.value - lastELem.value)
                / (::convertPointToBidimensional(currentElem.point) - ::convertPointToBidimensional(lastELem.point)));
            maxDiff = std::max(maxDiff, diff);
        }

        auto m = _params.rCoeff * maxDiff;

        if (m == 0) {
            m = 1;
        }

        PointType maxValueInterval = PointType();
        auto desiredInterval = ++_checkedPoints.begin();
        for (auto iter = ++_checkedPoints.begin(); iter != _checkedPoints.end(); iter++)
        {
            TrialPoint currentElem = *(iter--);
            TrialPoint lastELem = *(iter++);

            PointType temporaryValue = m * (::convertPointToBidimensional(currentElem.point) - ::convertPointToBidimensional(lastELem.point));
            PointType valueInterval = temporaryValue + (currentElem.value - lastELem.value) * (currentElem.value - lastELem.value)
                / temporaryValue - 2 * (currentElem.value + lastELem.value);

            if (iter == ++_checkedPoints.begin() || valueInterval > maxValueInterval) {
                maxValueInterval = valueInterval;
                desiredInterval = iter;
            }
        }

        TrialPoint currentElemDesiredInterval = *(desiredInterval--);
        TrialPoint lastElemDesiredInterval = *(desiredInterval++);

        Point newCoordinate = Point{ (::convertPointToBidimensional(currentElemDesiredInterval.point) + ::convertPointToBidimensional(lastElemDesiredInterval.point)) * 0.5
            - (currentElemDesiredInterval.value - lastElemDesiredInterval.value) * 0.5 / m };
        PointType newValue = _task.getValue(newCoordinate);
        _checkedPoints.insert({ newCoordinate, newValue });

        _points.push_back(newCoordinate);

        if (newValue < _globalMinimum.value) {
            _globalMinimum = { newCoordinate, newValue };
        }

        stopÑondition = ::convertPointToBidimensional(currentElemDesiredInterval.point) - ::convertPointToBidimensional(lastElemDesiredInterval.point) <= _params.accuracy;
        _complexity.iterationCount++;
    }

    return _globalMinimum;
}

Points BidimensionalGlobalSearch::getPoints() {
    if (_points.empty()) {
        for (TrialPoint trialPoint : _checkedPoints) {
            _points.push_back(trialPoint.point);
        }
    }
    return _points;
}

Complexity BidimensionalGlobalSearch::getComplexity() {
    return _complexity;
}

void BidimensionalGlobalSearch::clearCache() {
    _complexity = Complexity();
    _checkedPoints.clear();
    _points.clear();
}
