#pragma once

#include "algorithm_configurator.hpp"


AlgorithmConfigurator::AlgorithmConfigurator(int argc, char* argv[], std::function<void(const std::string&)> logger) :
    _grishaginTaskGenerator(), _algorithmsMap(), _logger(logger) {
    int internalArgsCount = argc - 1;
    if (internalArgsCount < constants::REQUIRED_ARGS_COUNT) {
        _logger("CONFIGURATION COMPLETED WITH ERROR:\nINSUFFICIENT NUMBER OF ARGUMENTS.\n");
        throw errors::INCORRECT_ARGS_COUNT_ERR_CODE;
    }
    double reliability = internalArgsCount < 3 ? constants::DEFAULT_RELIABILITY : std::atof(argv[3]);
    double accuracy = internalArgsCount < 4 ? constants::DEFAULT_ACCURACY : std::atof(argv[4]);
    double epsilonReserved = internalArgsCount < 5 ? constants::DEFAULT_EPSILON_RESERVED : std::atof(argv[5]);
    int densityScan = internalArgsCount < 6 ? constants::DEFAULT_MAPD_M : std::atoi(argv[6]);
    int keyScan = internalArgsCount < 7 ? constants::DEFAULT_MAPD_KEY : std::atoi(argv[7]);

    IndexAlgorithmParams algParams(reliability, accuracy, epsilonReserved);
    ScanParams scanParams(densityScan, keyScan);
    int startTaskNumber, endTaskNumber;
    if (std::string(argv[2]) == constants::ALL_TASK_KEY) {
        startTaskNumber = 0;
        endTaskNumber = _grishaginTaskGenerator.GetFamilySize();
    } else {
        startTaskNumber = std::atoi(argv[2]);
        endTaskNumber = startTaskNumber + 1;
    }

    for (int i = startTaskNumber; i < endTaskNumber; i++) {
        if (std::string(argv[1]) == constants::INDEX_ALG_KEY) {
            _algorithmsMap[i] = new IndexAlgorithm(_grishaginTaskGenerator[i], algParams, scanParams);
        } else if (std::string(argv[1]) == constants::MODIFIED_INDEX_ALG_KEY) {
            _algorithmsMap[i] = new ModifiedIndexAlgorithm(_grishaginTaskGenerator[i], algParams, scanParams);
        } else {
            _logger("CONFIGURATION COMPLETED WITH ERROR:\nUNSUPPORTED TASK TYPE.\n");
            throw errors::INCORRECT_ARGS_VALUE_ERR_CODE;
        }
    }

    _logger("CONFIGURATION SUCCESSFULLY COMPLETED.\n");
}

IConstrainedOptProblem* AlgorithmConfigurator::getTask(int taskNumber) {
    if (taskNumber < 0 || taskNumber >= _grishaginTaskGenerator.GetFamilySize()) {
        throw errors::POINT_OUT_OF_RANGE_ERR_CODE;
    }
    return _grishaginTaskGenerator[taskNumber];
}

void AlgorithmConfigurator::run() {
    int a = 2;
    for (const auto& [taskNumber, algorithm]: _algorithmsMap) {
        _logger("CALCULATION OPTIMUM TASK <" + std::to_string(taskNumber) + "> SUCCESSFULLY STARTED.\n");
        auto result = algorithm->run();
        _logger("FOUND OPTIMUM: " + 
            getPointDescription(result.point, result.value) + "\n");
        _logger("EXPECTED OPTIMUM: " +
            getPointDescription(_grishaginTaskGenerator[taskNumber]->GetOptimumPoint(),
                _grishaginTaskGenerator[taskNumber]->GetOptimumValue()) + "\n");
        _logger("MAX DEVIATION: " + 
            std::to_string(utils::getMaxCoordinateDifference(result.point, _grishaginTaskGenerator[taskNumber]->GetOptimumPoint())) + "\n");
        _logger("ITERATION COUNT: " + std::to_string(algorithm-> getComplexity().getIterationCount()) + "\n");
        _logger("CALCULATION COUNT: " + 
            getCalculationCountDescription(algorithm->getComplexity().getFunctionsCalculationCount()) + "\n");
        _logger("\n");
    }
}

std::string AlgorithmConfigurator::getPointDescription(Point point, PointType value) {
    std::string pointDescription = "(";
    for (PointType coordinate : point) {
        pointDescription += std::to_string(coordinate) + ", ";
    }
    pointDescription += std::to_string(value) + ")";
    return pointDescription;
}

std::string AlgorithmConfigurator::getCalculationCountDescription(std::vector<long> calculationCounts) {
    std::string calculationCount;
    for (long count : calculationCounts) {
        calculationCount += std::to_string(count) + ", ";
    }
    return calculationCount;
}

std::map<int, Points> AlgorithmConfigurator::getPoints() {
    std::map<int, Points> pointsMap;
    for (const auto [taskNumber, algorithm] : _algorithmsMap) {
        pointsMap[taskNumber] = algorithm->getPoints();
    }
    return pointsMap;
}