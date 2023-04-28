#pragma once

#include "models.hpp"
#include <functional>
#include <fstream>
#include <string>


namespace writer {
    void writePointsToFile(const std::string& fileName, Point startPoint, Point finishPoint, PointType step,
        const std::function<PointType(Point)> getValue);
}