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
* Configuration Contract:
* Input Format: {key}:{value}
* Required Keys:
* {ALG_TYPE}:{Type of algorithm used to solve the task (IA or MIA). Data type: string}
* Optional Keys:
* {TASK_NUM}:{Number of the task to be solved (number from 0 to 99). Data type: int. By default will be solved all tasks}
* {TASK_R}:{Task parameter: reliability parameter (number greater than one). Data type: double. By default is constants::DEFAULT_TASK_RELIABILITY}
* {TASK_ACC}:{Task parameter: accuracy. Data type: double. By default is constants::DEFAULT_TASK_ACCURACY}
* {TASK_EPS}:{Task parameter: delta (epsilon reserved). Data type: double. By default is constants::DEFAULT_TASK_EPSILON_RESERVED}
* {SCAN_DEN}:{Scan parameter: density of the scan construction. Data type: int. By default is constants::DEFAULT_SCAN_DENSITY = 10}
* {SCAN_KEY}:{Scan parameter: key of the scan construction. Data type: int. By default is constants::DEFAULT_SCAN_KEY = 10}
* {IS_PRINT}:{Whether to print function values to a file. Data type: bool. By default is constants::DEFAULT_IS_PRINT}
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