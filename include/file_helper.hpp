#pragma once

#include "template_task.hpp"
#include "utils.hpp"
#include <functional>
#include <fstream>
#include <string>


namespace parser {
    std::vector<std::string> parseDirectories(const std::string& pathsFilePath, const std::string& pathsFileName);
    std::string parseFileName(const std::string& nameContractFilePath, const std::string& nameContractFileName, const std::string& taskId);
    TemplateTask parseCustomTask(const std::string& configFilePath, const std::string& configFileName, const std::string& customTaskFileName);
}

namespace writer {
    void writePointsToFile(const std::string& fileName, Points points, const std::function<PointType(Point)>& getValue);
    void writePointIntervalToFile(const std::string& fileName, Point startPoint, Point finishPoint, PointType step,
        const std::function<PointType(Point)>& getValue, const std::function<bool(Point)>& condition);
}