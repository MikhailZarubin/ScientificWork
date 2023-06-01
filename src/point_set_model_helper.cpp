#pragma once

#include "point_set_model_helper.hpp"

void PointSetModelHelper::addNewPoint(IndexAlgorithmPointSetModel* pointSetModel, PointType point) {
    pointSetModel->newPoint = point;
}

bool PointSetModelHelper::isNeededReview(const IndexAlgorithmPointSetModel& pointSetModel) {
    return pointSetModel.newPoint.has_value();
}

PointType PointSetModelHelper::getNewPoint(const IndexAlgorithmPointSetModel& pointSetModel) {
    if (!pointSetModel.newPoint.has_value()) {
        throw ErrorWrapper(Errors::POINT_SET_MODEL_HELPER_ERROR, "[POINT SET MODEL HELPER] ATTEMPTED GET NEW POINT WHEN NEW POINT IS ABSENCE.\n");
    }
    return pointSetModel.newPoint.value();
}

void PointSetModelHelper::newPointReviewed(IndexAlgorithmPointSetModel* pointSetModel) {
    if (!pointSetModel->newPoint.has_value()) {
        throw ErrorWrapper(Errors::POINT_SET_MODEL_HELPER_ERROR, "[POINT SET MODEL HELPER] ATTEMPTED REVIEW NEW POINT WHEN NEW POINT IS ABSENCE.\n");
    }
    pointSetModel->reviewedPoints.insert(pointSetModel->newPoint.value());
    pointSetModel->newPoint.reset();
}