#pragma once

#include "algorithm_configurator.hpp"


AlgorithmConfigurator::AlgorithmConfigurator(int argc, char* argv[], std::function<void(const std::string&)> logger) :
    _constrainedProblemFamily(nullptr), _algorithmsMap(), _logger(logger) {
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
    std::string algType = configurationMap[constants::KEY_ALG_TYPE];

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
    int iterationLimit = utils::contains(configurationMap, constants::KEY_ITERATION_LIMIT) ?
        std::atoi(configurationMap[constants::KEY_ITERATION_LIMIT].c_str()) : constants::DEFAULT_ITERATION_LIMIT;

    if (utils::contains(configurationMap, constants::KEY_PRINT_LEVEL) && 
        (std::atoi(configurationMap[constants::KEY_PRINT_LEVEL].c_str()) < static_cast<int>(constants::PrintLevel::PRINT_NOT_ANYTHING) ||
        std::atoi(configurationMap[constants::KEY_PRINT_LEVEL].c_str()) > static_cast<int>(constants::PrintLevel::PRINT_ALL_POINTS))) {
        _logger("CONFIGURATION COMPLETED WITH ERROR.\nINVALID PRINT LEVEL RECEIVED: " + configurationMap[constants::KEY_PRINT_LEVEL] + "\n");
        throw errors::INCORRECT_ARGS_VALUE_ERR_CODE;
    } else {
        _printLevel = utils::contains(configurationMap, constants::KEY_PRINT_LEVEL) ?
            static_cast<constants::PrintLevel>(std::atoi(configurationMap[constants::KEY_PRINT_LEVEL].c_str())) : constants::DEFAULT_PRINT_LEVEL;
    }

    GlobalSearchAlgorithmParams globalSearchAlgParams(reliability, accuracy);
    IndexAlgorithmParams indexAlgParams(reliability, accuracy, epsilonReserved, iterationLimit);
    ScanParams scanParams(densityScan, keyScan);
    if (utils::contains(configurationMap, constants::KEY_CUSTOM_TASK)) {
        _algorithmsMap[constants::CUSTOM_TASK_NUMBER] = createAlgorithm(algType,
            parser::parseCustomTask(constants::API_DIR, constants::CONFIG_PATH_FILE,
                configurationMap[constants::KEY_CUSTOM_TASK]), globalSearchAlgParams, indexAlgParams, scanParams);
    }
    else {
        _constrainedProblemFamily = new TGrishaginConstrainedProblemFamily();
        int startTaskNumber, endTaskNumber;
        if (utils::contains(configurationMap, constants::KEY_TASK_NUMBER)) {
            startTaskNumber = std::atoi(configurationMap[constants::KEY_TASK_NUMBER].c_str());
            endTaskNumber = startTaskNumber + 1;
        }
        else {
            startTaskNumber = 0;
            endTaskNumber = _constrainedProblemFamily->GetFamilySize();
        }

        for (int i = startTaskNumber; i < endTaskNumber; i++) {
            _algorithmsMap[i] = createAlgorithm(algType, TemplateTask(_constrainedProblemFamily->operator[](i),
                TrialPoint(_constrainedProblemFamily->operator[](i)->GetOptimumPoint(), _constrainedProblemFamily->operator[](i)->GetOptimumValue())),
                globalSearchAlgParams, indexAlgParams, scanParams);
        }
    }

    std::vector<std::string> paths = parser::parseDirectories(constants::API_DIR, constants::DATA_PATHS_FILE);
    if (paths.size() != constants::DATA_PATHS_COUNT) {
        _logger("API FILES CHANGED! CONFIGURATION CANNOT BE COMPLETED.\n");
        throw errors::API_CHANGED_ERR_CODE;
    }
    _algorithmPointsDir = paths[0];
    _functionPointsDir = paths[1];
    _invalidPointsDir = paths[2];

    _logger("CONFIGURATION SUCCESSFULLY COMPLETED.\n");
}


Algorithm* AlgorithmConfigurator::createAlgorithm(const std::string& algType,
    const TemplateTask& templateTask, const GlobalSearchAlgorithmParams& globalSearchAlgParams,
    const IndexAlgorithmParams& indexAlgParams, const ScanParams& scanParams) {
    if (algType == constants::DEFINE_INDEX_ALG) {
        return new IndexAlgorithm(templateTask, indexAlgParams, scanParams);
    }
    else if (algType == constants::DEFINE_MODIFIED_INDEX_ALG) {
        return new ModifiedIndexAlgorithm(templateTask, indexAlgParams, scanParams);
    }
    else if (algType == constants::DEFINE_GLOBAL_SEARCH_ALG) {
        return new BidimensionalGlobalSearch(templateTask, globalSearchAlgParams);
    }
    else {
        _logger("CONFIGURATION COMPLETED WITH ERROR.\nUNSUPPORTED TASK TYPE: " + algType + "\n");
        throw errors::INCORRECT_ARGS_VALUE_ERR_CODE;
    }
}

void AlgorithmConfigurator::run() {
    PointType maxDeviation = -DBL_MAX;
    for (const auto& [taskNumber, algorithm]: _algorithmsMap) {
        _logger("CALCULATION OPTIMUM TASK <" + std::to_string(taskNumber) + "> SUCCESSFULLY STARTED.\n");
        auto result = algorithm->run();
        maxDeviation = std::max(maxDeviation, utils::getMaxCoordinateDifference(result.point, _algorithmsMap[taskNumber]->getTask().getOptimumPoint()));
        _logger("FOUND OPTIMUM: " + 
            getPointDescription(result.point, result.value) + "\n");
        _logger("EXPECTED OPTIMUM: " +
            getPointDescription(_algorithmsMap[taskNumber]->getTask().getOptimumPoint(),
                _algorithmsMap[taskNumber]->getTask().getOptimumValue()) + "\n");
        _logger("MAX DEVIATION: " + 
            std::to_string(utils::getMaxCoordinateDifference(result.point, _algorithmsMap[taskNumber]->getTask().getOptimumPoint())) + "\n");
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
    for (int i = 0; i < calculationCounts.size(); i++) {
        calculationCount += std::to_string(calculationCounts[i]);
        if (i != calculationCounts.size() - 1) {
            calculationCount += ", ";
        }
    }
    return calculationCount;
}

void AlgorithmConfigurator::printPointsToFile(int taskNumber, Points points) {
    Borders borders = _algorithmsMap[taskNumber]->getTask().getTaskBorders();
    std::string pointsFileName = parser::parseFileName(constants::API_DIR, constants::NAME_CONTRACT_FILE, taskNumber);
    switch (_printLevel) {
    case constants::PrintLevel::PRINT_ALL_POINTS:
        writer::writePointIntervalToFile(_functionPointsDir + pointsFileName, borders.leftBorder, borders.rightBorder,
            constants::STEP_PRINT_POINTS, [&](Point point) { return _algorithmsMap[taskNumber]->getTask().getTaskValue(point); },
            [&](Point point) { return true; });
        writer::writePointIntervalToFile(_invalidPointsDir + pointsFileName, borders.leftBorder, borders.rightBorder,
            constants::STEP_PRINT_POINTS, [&](Point point) { return _algorithmsMap[taskNumber]->getTask().getTaskValue(point); },
            [&](Point point) {
                bool isValid = true;
                for (int i = 0; i < _algorithmsMap[taskNumber]->getTask().getConstraintsCount(); i++) {
                    if (_algorithmsMap[taskNumber]->getTask().getConstraintValue(i, point) > 0) {
                        isValid = false;
                        break;
                    }
                }
                return !isValid;
            });
    case constants::PrintLevel::PRINT_ONLY_TRIAL_POINT:
        points.push_back(_algorithmsMap[taskNumber]->getTask().getOptimumPoint());
        writer::writePointsToFile(_algorithmPointsDir + pointsFileName,
            points, [&](Point point) { return _algorithmsMap[taskNumber]->getTask().getTaskValue(point); });
        break;
    default:
        break;
    }
}


AlgorithmConfigurator::~AlgorithmConfigurator() {
    if (_constrainedProblemFamily != nullptr) {
        delete _constrainedProblemFamily;
    }
    for (auto& [_, algorithm] : _algorithmsMap) {
        delete algorithm;
    }
}
