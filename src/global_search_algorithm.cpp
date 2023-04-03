#pragma once

#include "global_search_algorithm.hpp"

namespace {
    PointType convertPointToBidimensional(Point point) {
        if (point.size() != constants::BIDIMENSIONAL_POINT_SIZE) {
            throw errors::INCORRECT_POINT_SIZE_ERR_CODE;
        }
        return point[constants::BIDIMENSIONAL_POINT_SIZE - 1];
    }

    bool comparisonPoints(const TrialPoint& firstPoint, const TrialPoint& secondPoint) {
        return convertPointToBidimensional(firstPoint.point) < convertPointToBidimensional(secondPoint.point);
    }
}


BidimensionalGlobalSearch::BidimensionalGlobalSearch(const Function& task, const GlobalSearchAlgorithmParams& params) :
    _task(task), _params(params), _globalMinimum(), _points(), _complexity(), _checkedPoints(::comparisonPoints) {}

void BidimensionalGlobalSearch::startIteration() {
    Point leftBorder = _task.getLeftBorder();
    Point rightBorder = _task.getRightBorder();
    PointType valueFuncLeftBorder = _task.getValue(leftBorder);
    PointType valueFuncRightBorder = _task.getValue(rightBorder);

    if (valueFuncLeftBorder <= valueFuncRightBorder) {
        _globalMinimum = TrialPoint(leftBorder, valueFuncLeftBorder);
    } else {
        _globalMinimum = TrialPoint(rightBorder, valueFuncRightBorder);
    }

    _checkedPoints.insert({ leftBorder, valueFuncLeftBorder });
    _checkedPoints.insert({ rightBorder, valueFuncRightBorder });

    _points.push_back(leftBorder);
    _points.push_back(rightBorder);
}

TrialPoint BidimensionalGlobalSearch::run() {
    clearData();
    startIteration();

    bool stop�ondition = false;
    PointType maxDiff = PointType();
    while (!stop�ondition) {
        for (auto iter = ++_checkedPoints.begin(); iter != _checkedPoints.end(); iter++) {
            TrialPoint currentElem = *(iter--);
            TrialPoint lastELem = *(iter++);

            PointType diff = abs((currentElem.value - lastELem.value)
                / (::convertPointToBidimensional(currentElem.point) - ::convertPointToBidimensional(lastELem.point)));
            maxDiff = std::max(maxDiff, diff);
        }

        auto m = _params.rCoeff * maxDiff;

        if (maxDiff == 0.0) {
            m = 1.0;
        }

        PointType maxR = PointType();
        auto desiredInterval = ++_checkedPoints.begin();
        for (auto iter = ++_checkedPoints.begin(); iter != _checkedPoints.end(); iter++) {
            TrialPoint currentElem = *(iter--);
            TrialPoint lastElem = *(iter++);

            PointType pointInterval = ::convertPointToBidimensional(currentElem.point) - ::convertPointToBidimensional(lastElem.point);
            PointType valueInterval = currentElem.value - lastElem.value;
            auto R = m * pointInterval + (valueInterval * valueInterval) / (m * pointInterval) - 2.0 * (currentElem.value + lastElem.value);

            if (iter == ++_checkedPoints.begin() || R > maxR) {
                maxR = R;
                desiredInterval = iter;
            }
        }

        TrialPoint currentElemDesiredInterval = *(desiredInterval--);
        TrialPoint lastElemDesiredInterval = *(desiredInterval++);

        Point newCoordinate = Point{ 0.5 * (
            ::convertPointToBidimensional(currentElemDesiredInterval.point) + ::convertPointToBidimensional(lastElemDesiredInterval.point)
            + ((1.0 / m) * (currentElemDesiredInterval.value - lastElemDesiredInterval.value))) };
        PointType newValue = _task.getValue(newCoordinate);

        _checkedPoints.insert({ newCoordinate, newValue });
        _points.push_back(newCoordinate);

        if (newValue < _globalMinimum.value) {
            _globalMinimum = { newCoordinate, newValue };
        }

        stop�ondition = ::convertPointToBidimensional(currentElemDesiredInterval.point) - ::convertPointToBidimensional(lastElemDesiredInterval.point) <= _params.accuracy;
        _complexity.incrementIteration();
    }

    return _globalMinimum;
}

Points BidimensionalGlobalSearch::getPoints() {
    return _points;
}

Complexity BidimensionalGlobalSearch::getComplexity() {
    return _complexity;
}

void BidimensionalGlobalSearch::clearData() {
    _complexity = Complexity();
    _checkedPoints.clear();
    _points.clear();
}
