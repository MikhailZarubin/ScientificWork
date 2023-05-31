#pragma once

#include "../GCGen/sample_src/Grishagin/GrishaginConstrainedProblemFamily.hpp"
#include "file_helper.hpp"
#include "modified_index_algorithm.hpp"
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
* {TASK_R}:{Task parameter: reliability (number greater than 1). Data type: double. By default is constants::DEFAULT_TASK_RELIABILITY}
* {TASK_ACC}:{Task parameter: accuracy. Data type: double. By default is constants::DEFAULT_TASK_ACCURACY}
* {TASK_EPS}:{Task parameter: delta (epsilon reserved). Data type: double. By default is constants::DEFAULT_TASK_EPSILON_RESERVED}
* {SCAN_DEN}:{Scan parameter: density of the scan construction. Data type: int. By default is constants::DEFAULT_SCAN_DENSITY}
* {SCAN_KEY}:{Scan parameter: key of the scan construction. Data type: int. By default is constants::DEFAULT_SCAN_KEY}
* {PRINT_LEVEL}:{Whether to print points to the file. 0 - do not print anything, 1 - print only trial points, 2 - print trial points and functions points.
* Data type: int. By default is constants::DEFAULT_PRINT_LEVEL}
* {ITER_LIMIT}:{Limit of iterations of the algorithm to solve the task. Data type: int. By default is constants::ITERATION_LIMIT}
* {CUSTOM_TASK}:{File with model of task which should be created by descripted pattern in configuration guide. Data type: string. By default is not defined}
*/


class AlgorithmConfigurator {
    IConstrainedOptProblemFamily* _constrainedProblemFamily;
    std::map<int, Algorithm*> _algorithmsMap;
    std::function<void(const std::string)> _logger;
    constants::PrintLevel _printLevel;

    std::string _algorithmPointsDir;
    std::string _functionPointsDir;
    std::string _invalidPointsDir;

    Algorithm* createAlgorithm(const std::string& algType,
        const TemplateTask& templateTask, const IndexAlgorithmParams& algParams, const ScanParams& scanParams);

    std::string getPointDescription(Point point, PointType value);
    std::string getCalculationCountDescription(std::vector<long> calculationCounts);
    void printPointsToFile(int taskNumber, Points points);
public:
    AlgorithmConfigurator(int argc, char* argv[], std::function<void(const std::string&)> logger);
    void run();
    ~AlgorithmConfigurator();
};