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
        throw errors::POINT_SET_MODEL_ERROR_ERR_CODE;
    }
    return pointSetModel.newPoint.value();
}

void PointSetModelHelper::newPointReviewed(IndexAlgorithmPointSetModel* pointSetModel) {
    if (!pointSetModel->newPoint.has_value()) {
        throw errors::POINT_SET_MODEL_ERROR_ERR_CODE;
    }
    pointSetModel->reviewedPoints.insert(pointSetModel->newPoint.value());
    pointSetModel->newPoint.reset();
}