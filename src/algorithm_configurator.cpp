#pragma once

#include "algorithm_configurator.hpp"


AlgorithmConfigurator::AlgorithmConfigurator(int argc, char* argv[], std::function<void(const std::string&)> logger) :
    _grishaginTaskGenerator(), _algorithmsMap(), _logger(logger) {
    std::map<std::string, std::string> configurationMap;
    std::vector<std::string> splitingStr;
    for (int i = 1; i < argc; i++) {
        splitingStr = utils::split(std::string(argv[i]), ":");
        if (splitingStr.size() != 2) {
            _logger("CONFIGURATION COMPLETED WITH ERROR.\nINVALID ARGUMENT RECEIVED: " + std::string(argv[i]) + "\n");
            throw errors::INCORRECT_ARGS_VALUE_ERR_CODE;
        }
        configurationMap[splitingStr[0]] = splitingStr[1];
    }
    
    if (!utils::contains(configurationMap, constants::KEY_ALG_TYPE)) {
        _logger("CONFIGURATION COMPLETED WITH ERROR.\nMISSING IS ALGORITHM TYPE IN ARGUMENTS\n");
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

    if (utils::contains(configurationMap, constants::KEY_PRINT_LEVEL) && 
        (std::atoi(configurationMap[constants::KEY_PRINT_LEVEL].c_str()) < static_cast<int>(constants::PrintLevel::PRINT_NOT_ANYTHING) ||
        std::atoi(configurationMap[constants::KEY_PRINT_LEVEL].c_str()) > static_cast<int>(constants::PrintLevel::PRINT_ALL_POINTS))) {
        _logger("CONFIGURATION COMPLETED WITH ERROR.\nINVALID PRINT LEVEL RECEIVED: " + configurationMap[constants::KEY_PRINT_LEVEL] + "\n");
        throw errors::INCORRECT_ARGS_VALUE_ERR_CODE;
    } else {
        _printLevel = utils::contains(configurationMap, constants::KEY_PRINT_LEVEL) ?
            static_cast<constants::PrintLevel>(std::atoi(configurationMap[constants::KEY_PRINT_LEVEL].c_str())) : constants::DEFAULT_PRINT_LEVEL;
    }

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
            _logger("CONFIGURATION COMPLETED WITH ERROR.\nUNSUPPORTED TASK TYPE: " + configurationMap[constants::KEY_ALG_TYPE] + "\n");
            throw errors::INCORRECT_ARGS_VALUE_ERR_CODE;
        }
    }

    std::vector<std::string> paths = parser::parseDirectories(constants::API_DIR, constants::PATHS_FILE);
    if (paths.size() != constants::PATHS_COUNT) {
        _logger("API FILES CHANGED! CONFIGURATION CANNOT BE COMPLETED.\n");
        throw errors::API_CHANGED_ERR_CODE;
    }
    _algorithmPointsDir = paths[0];
    _functionPointsDir = paths[1];
    _invalidPointsDir = paths[2];

    _logger("CONFIGURATION SUCCESSFULLY COMPLETED.\n");
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

        printPointsToFile(taskNumber, algorithm->getPoints());
    }
    _logger("MAX DEVIATION AMONG ALL TASKS: " + std::to_string(maxDeviation) + "\n");
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

void AlgorithmConfigurator::printPointsToFile(int taskNumber, Points points) {
    Point leftBorder, rightBorder;
    std::string pointsFileName = parser::parseFileName(constants::API_DIR, constants::NAME_CONTRACT_FILE, taskNumber);
    switch (_printLevel) {
    case constants::PrintLevel::PRINT_ALL_POINTS:
        _grishaginTaskGenerator[taskNumber]->GetBounds(leftBorder, rightBorder);
        writer::writePointIntervalToFile(_functionPointsDir + pointsFileName, leftBorder, rightBorder,
            constants::STEP_PRINT_POINTS, [&](Point point) { return _grishaginTaskGenerator[taskNumber]->ComputeFunction(point); },
            [&](Point point) { return true; });
        writer::writePointIntervalToFile(_invalidPointsDir + pointsFileName, leftBorder, rightBorder,
            constants::STEP_PRINT_POINTS, [&](Point point) { return _grishaginTaskGenerator[taskNumber]->ComputeFunction(point); },
            [&](Point point) {
                bool isValid = true;
                for (int i = 0; i < _grishaginTaskGenerator[taskNumber]->GetConstraintsNumber(); i++) {
                    if (_grishaginTaskGenerator[taskNumber]->ComputeConstraint(i, point) > 0) {
                        isValid = false;
                        break;
                    }
                }
                return !isValid;
            });
    case constants::PrintLevel::PRINT_ONLY_TRIAL_POINT:
        points.push_back(_grishaginTaskGenerator[taskNumber]->GetOptimumPoint());
        writer::writePointsToFile(_algorithmPointsDir + pointsFileName,
            points, [&](Point point) { return _grishaginTaskGenerator[taskNumber]->ComputeFunction(point); });
        break;
    default:
        break;
    }
}
