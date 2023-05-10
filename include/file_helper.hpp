#pragma once

#include "errors.hpp"
#include "models.hpp"
#include "utils.hpp"
#include <functional>
#include <fstream>
#include <string>


namespace parser {
    std::vector<std::string> parseDirectories(const std::string& pathsFilePath, const std::string& pathsFileName);
    std::string parseFileName(const std::string& nameContractFilePath, const std::string& nameContractFileName, int taskNumber);
}

namespace writer {
    void writePointsToFile(const std::string& fileName, Points points, const std::function<PointType(Point)>& getValue);
    void writePointIntervalToFile(const std::string& fileName, Point startPoint, Point finishPoint, PointType step,
        const std::function<PointType(Point)>& getValue, const std::function<bool(Point)>& condition);
}