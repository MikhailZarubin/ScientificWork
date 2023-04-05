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

GlobalSearchAlgorithmParams::GlobalSearchAlgorithmParams(double rCoeff, double accuracy) {
    if (rCoeff <= constants::MIN_R) {
        throw errors::INCORRECT_PARAM_ERR_CODE;
    }

    this->rCoeff = rCoeff;
    this->accuracy = accuracy;
}

GlobalSearchAlgorithmParams::GlobalSearchAlgorithmParams(const GlobalSearchAlgorithmParams& params) : GlobalSearchAlgorithmParams(params.rCoeff, params.accuracy) {}

IndexAlgorithmParams::IndexAlgorithmParams(double rCoeff, double accuracy, double delta) {
    if (rCoeff <= constants::MIN_R) {
        throw errors::INCORRECT_PARAM_ERR_CODE;
    }

    this->rCoeff = rCoeff;
    this->accuracy = accuracy;
    this->delta = delta;
}

IndexAlgorithmParams::IndexAlgorithmParams(const IndexAlgorithmParams& params) : IndexAlgorithmParams(params.rCoeff, params.accuracy, params.delta) {}

Borders::Borders(const Point& leftBorder, const Point& rightBorder) {
    if (rightBorder < leftBorder) {
        throw errors::INCORRECT_BORDERS_ERR_CODE;
    }
    this->leftBorder = leftBorder;
    this->rightBorder = rightBorder;
}

Borders::Borders(const Borders& borders) : Borders(borders.leftBorder, borders.rightBorder) {}

IndexAlgorithmStepResult::IndexAlgorithmStepResult(PointType point, std::size_t v, PointType z) {
    this->point = point;
    this->v = v;
    this->z = z;
}

IndexAlgorithmStepResult::IndexAlgorithmStepResult(const IndexAlgorithmStepResult& indexAlgorithmStepResult) :
    IndexAlgorithmStepResult(indexAlgorithmStepResult.point, indexAlgorithmStepResult.v, indexAlgorithmStepResult.z) {}

IndexAlgorithmPointSetModel::IndexAlgorithmPointSetModel(std::set<PointType> reviewedPoints, std::optional<PointType> newPoint) {
    this->reviewedPoints = reviewedPoints;
    this->newPoint = newPoint;
}

IndexAlgorithmPointSetModel::IndexAlgorithmPointSetModel(const IndexAlgorithmPointSetModel& indexAlgorithmPointSetModel) :
    IndexAlgorithmPointSetModel(indexAlgorithmPointSetModel.reviewedPoints, indexAlgorithmPointSetModel.newPoint) {}