#pragma once

#include "../GCGen/sample_src/Grishagin/GrishaginConstrainedProblemFamily.hpp"
#include "base_algorithm.hpp"
#include "constants.hpp"
#include "errors.hpp"
#include "file_helper.hpp"
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
* {PRINT_LEVEL}:{Whether to print points to a file. 0 - do not print anything, 1 - print only trial points, 2 - print trial points and functions points. Data type: int. By default is constants::DEFAULT_PRINT_LEVEL}
*/


class AlgorithmConfigurator {
    TGrishaginConstrainedProblemFamily _grishaginTaskGenerator;
    std::function<void(const std::string)> _logger;
    std::map<int, Algorithm*> _algorithmsMap;
    constants::PrintLevel _printLevel;

    std::string _algorithmPointsDir;
    std::string _functionPointsDir;
    std::string _invalidPointsDir;

    std::string getPointDescription(Point point, PointType value);
    std::string getCalculationCountDescription(std::vector<long> calculationCounts);
    void printPointsToFile(int taskNumber, Points points);
public:
    AlgorithmConfigurator(int argc, char* argv[], std::function<void(const std::string&)> logger);
    void run();
};