#pragma once

#include "models.hpp"


TrialPoint::TrialPoint(Point point, PointType value) {
    this->point = point;
    this->value = value;
}

TrialPoint::TrialPoint(const TrialPoint& pointResult) : TrialPoint(pointResult.point, pointResult.value) {}

TrialPoint& TrialPoint::operator = (const TrialPoint& trialPoint) {
    this->point = trialPoint.point;
    this->value = trialPoint.value;
    return *this;
}

Complexity::Complexity(long int iterationCount) {
    this->iterationCount = iterationCount;
}

Complexity::Complexity(const Complexity& complexity) : Complexity(complexity.iterationCount) {}

GlobalSearchTaskParams::GlobalSearchTaskParams(double rCoeff, double accuracy) {
    if (rCoeff <= constants::MIN_R) {
        throw errors::INCORRECT_PARAM_ERR_CODE;
    }

    this->rCoeff = rCoeff;
    this->accuracy = accuracy;
}

GlobalSearchTaskParams::GlobalSearchTaskParams(const GlobalSearchTaskParams& params) : GlobalSearchTaskParams(params.rCoeff, params.accuracy) {}

Borders::Borders(const Point& leftBorder, const Point& rightBorder) {
    if (rightBorder < leftBorder) {
        throw errors::INCORRECT_BORDERS_ERR_CODE;
    }
    this->leftBorder = leftBorder;
    this->rightBorder = rightBorder;
}

Borders::Borders(const Borders& borders) : Borders(borders.leftBorder, borders.rightBorder) {}