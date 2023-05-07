#pragma once

#include "../GCGen/sample_src/Grishagin/GrishaginConstrainedProblemFamily.hpp"
#include "base_algorithm.hpp"
#include "constants.hpp"
#include "errors.hpp"
#include "index_algorithm.hpp"
#include "modified_index_algorithm.hpp"
#include "utils.hpp"
#include <functional>
#include <math.h>
#include <map>

/*
* CONFIGURATION CONTRACT
* Required Arguments:
* argv[1] - Type of algorithm used to solve the task (IA or MIA).
* argv[2] - Number of the task to be solved (ALL or number from 0 to 99).
* Optional Arguments:
* argc[3] - Algorithm parameter: reliability parameter (double number greater than one). By default is constants::DEFAULT_RELIABILITY.
* argc[4] - Algorithm parameter: accuracy. By default is constants::DEFAULT_ACCURACY.
* argc[5] - Algorithm parameter: delta (epsilon reserved). By default is constants::DEFAULT_EPSILON_RESERVED.
* argc[6] - Scan parameter: density of the scan construction. By default is constants::DEFAULT_MAPD_M = 10.
* argc[7] - Scan parameter: key of the scan construction. By default is constants::DEFAULT_MAPD_KEY = 10.
*/


class AlgorithmConfigurator {
    TGrishaginConstrainedProblemFamily _grishaginTaskGenerator;
    std::function<void(const std::string)> _logger;
    std::map<int, Algorithm*> _algorithmsMap;

    std::string getPointDescription(Point point, PointType value);
    std::string getCalculationCountDescription(std::vector<long> calculationCounts);
public:
    AlgorithmConfigurator(int argc, char* argv[], std::function<void(const std::string&)> logger);
    IConstrainedOptProblem* getTask(int taskNumber);
    void run();
    std::map<int, Points> getPoints();
};