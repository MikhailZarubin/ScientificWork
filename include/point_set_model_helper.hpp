#pragma once

#include "models.hpp"
#include <optional>

namespace PointSetModelHelper {
    void addNewPoint(IndexAlgorithmPointSetModel* pointSetModel, PointType point);
    bool isNeededReview(const IndexAlgorithmPointSetModel& pointSetModel);
    PointType getNewPoint(const IndexAlgorithmPointSetModel& pointSetModel);
    void newPointReviewed(IndexAlgorithmPointSetModel* pointSetModel);
}