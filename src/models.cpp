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
        throw ErrorWrapper(Errors::MODELS_ERROR, "[MODELS] RELIABILITY PARAMETER IS <" + std::to_string(rCoeff) + ">. MININMUM RELIABILITY VALUE IS 1.\n");
    }

    if (accuracy <= constants::MIN_ACCURACY) {
        throw ErrorWrapper(Errors::MODELS_ERROR, "[MODELS] ACCURACY PARAMETER IS <" + std::to_string(accuracy) + ">. ACCURACY SHOULD BE GREATER THAN 0.\n");
    }

    this->rCoeff = rCoeff;
    this->accuracy = accuracy;
}

GlobalSearchAlgorithmParams::GlobalSearchAlgorithmParams(const GlobalSearchAlgorithmParams& params) : GlobalSearchAlgorithmParams(params.rCoeff, params.accuracy) {}

IndexAlgorithmParams::IndexAlgorithmParams(double rCoeff, double accuracy, double delta, int iterationLimit) {
    if (rCoeff <= constants::MIN_R) {
        throw ErrorWrapper(Errors::MODELS_ERROR, "[MODELS] RELIABILITY PARAMETER IS <" + std::to_string(rCoeff) + ">. MININMUM RELIABILITY VALUE IS 1.\n");
    }

    if (accuracy <= constants::MIN_ACCURACY) {
        throw ErrorWrapper(Errors::MODELS_ERROR, "[MODELS] ACCURACY PARAMETER IS <" + std::to_string(accuracy) + ">. ACCURACY SHOULD BE GREATER THAN 0.\n");
    }

    this->rCoeff = rCoeff;
    this->accuracy = accuracy;
    this->delta = delta;
    this->iterationLimit = iterationLimit;
}

IndexAlgorithmParams::IndexAlgorithmParams(const IndexAlgorithmParams& params) : IndexAlgorithmParams(params.rCoeff, params.accuracy, params.delta, params.iterationLimit) {}

ScanParams::ScanParams(int density, int key) {
    this->density = density;
    this->key = key;
}

ScanParams::ScanParams(const ScanParams& params) : ScanParams(params.density, params.key) {}

Borders::Borders(const Point& leftBorder, const Point& rightBorder) {
    if (rightBorder < leftBorder) {
        throw ErrorWrapper(Errors::MODELS_ERROR, "[MODELS] RIGHT BORDER IS SMALLER LEFT BORDER.\n");
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