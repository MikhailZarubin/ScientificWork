#pragma once

#include "algorithm_configurator.hpp"


AlgorithmConfigurator::AlgorithmConfigurator(int argc, char* argv[], std::function<void(const std::string&)> logger) :
    _grishaginTaskGenerator(), _algorithmsMap(), _logger(logger) {
    std::map<std::string, std::string> configurationMap;
    std::vector<std::string> splitingStr;
    for (int i = 1; i < argc; i++) {
        splitingStr = utils::split(std::string(argv[i]), ":");
        if (splitingStr.size() != 2) {
            _logger("CONFIGURATION COMPLETED WITH ERROR:\nINVALID ARGUMENT RECEIVED: " + std::string(argv[i]) + "\n");
            throw errors::INCORRECT_ARGS_VALUE_ERR_CODE;
        }
        configurationMap[splitingStr[0]] = splitingStr[1];
    }
    
    if (!utils::contains(configurationMap, constants::KEY_ALG_TYPE)) {
        _logger("CONFIGURATION COMPLETED WITH ERROR:\nMISSING IS ALGORITHM TYPE IN ARGUMENTS\n");
        throw errors::INCORRECT_ARGS_COUNT_ERR_CODE;
    }

    double reliability = utils::contains(configurationMap, constants::KEY_TASK_RELIABILITY) ?
        std::atof(configurationMap[constants::KEY_TASK_RELIABILITY].c_str()) : constants::DEFAULT_TASK_RELIABILITY;
    double accuracy = utils::contains(configurationMap, constants::KEY_TASK_ACCURACY) ?
        std::atof(configurationMap[constants::KEY_TASK_ACCURACY].c_str()) : constants::DEFAULT_TASK_ACCURACY;
    double epsilonReserved = utils::contains(configurationMap, constants::KEY_TASK_EPSILON_RESERVED) ?
        std::atof(configurationMap[constants::KEY_TASK_EPSILON_RESERVED].c_str()) : constants::DEFAULT_TASK_EPSILON_RESERVED;
    int densityScan = utils::contains(configurationMap, constants::KEY_SCAN_DENSITY) ?
        std::atoi(configurationMap[constants::KEY_SCAN_DENSITY].c_str()) : constants::DEFAULT_SCAN_DENSITY;
    int keyScan = utils::contains(configurationMap, constants::KEY_SCAN_KEY) ?
        std::atoi(configurationMap[constants::KEY_SCAN_KEY].c_str()) : constants::DEFAULT_SCAN_KEY;

    IndexAlgorithmParams algParams(reliability, accuracy, epsilonReserved);
    ScanParams scanParams(densityScan, keyScan);
    int startTaskNumber, endTaskNumber;
    if (utils::contains(configurationMap, constants::KEY_TASK_NUMBER)) {
        startTaskNumber = std::atoi(configurationMap[constants::KEY_TASK_NUMBER].c_str());
        endTaskNumber = startTaskNumber + 1;
    } else {
        startTaskNumber = 0;
        endTaskNumber = _grishaginTaskGenerator.GetFamilySize();
    }

    for (int i = startTaskNumber; i < endTaskNumber; i++) {
        if (configurationMap[constants::KEY_ALG_TYPE] == constants::DEFINE_INDEX_ALG) {
            _algorithmsMap[i] = new IndexAlgorithm(_grishaginTaskGenerator[i], algParams, scanParams);
        } else if (configurationMap[constants::KEY_ALG_TYPE] == constants::DEFINE_MODIFIED_INDEX_ALG) {
            _algorithmsMap[i] = new ModifiedIndexAlgorithm(_grishaginTaskGenerator[i], algParams, scanParams);
        } else {
            _logger("CONFIGURATION COMPLETED WITH ERROR:\nUNSUPPORTED TASK TYPE: " + configurationMap[constants::KEY_ALG_TYPE] + "\n");
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
    PointType maxDeviation = -DBL_MAX;
    for (const auto& [taskNumber, algorithm]: _algorithmsMap) {
        _logger("CALCULATION OPTIMUM TASK <" + std::to_string(taskNumber) + "> SUCCESSFULLY STARTED.\n");
        auto result = algorithm->run();
        maxDeviation = std::max(maxDeviation, utils::getMaxCoordinateDifference(result.point, _grishaginTaskGenerator[taskNumber]->GetOptimumPoint()));
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
    _logger("MAX DEVIATION AMOND ALL TASKS: " + std::to_string(maxDeviation) + "\n");
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