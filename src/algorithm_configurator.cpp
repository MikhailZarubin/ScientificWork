#pragma once

#include "algorithm_configurator.hpp"


AlgorithmConfigurator::AlgorithmConfigurator(int argc, char* argv[], Logger* logger) :
    _constrainedProblemFamily(nullptr), _algorithmsMap(utils::stringKeysComparator), _logger(logger) {
    if (_logger == nullptr) {
        throw ErrorWrapper(Errors::CONFIGURATION_ERROR, "[CONFIGURATOR] LOGGER IS NULL.\n");
    }

    std::map<std::string, std::string> configurationMap;
    std::vector<std::string> splitingStr;
    for (int i = 1; i < argc; i++) {
        splitingStr = utils::split(std::string(argv[i]), ":");
        if (splitingStr.size() != 2) {
            _logger->log("CONFIGURATION COMPLETED WITH ERROR.\n");
            throw ErrorWrapper(Errors::CONFIGURATION_ERROR, "[CONFIGURATOR] INVALID ARGUMENT RECEIVED: " + std::string(argv[i]) + "\n");
        }
        configurationMap[splitingStr[0]] = splitingStr[1];
    }
    
    if (!utils::contains(configurationMap, constants::KEY_ALG_TYPE)) {
        _logger->log("CONFIGURATION COMPLETED WITH ERROR.\n");
        throw ErrorWrapper(Errors::CONFIGURATION_ERROR, "[CONFIGURATOR] ALGORITHM TYPE IS MISSING IN ARGUMENTS.\n");
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
        _logger->log("CONFIGURATION COMPLETED WITH ERROR.\n");
        throw ErrorWrapper(Errors::CONFIGURATION_ERROR, "[CONFIGURATOR] INVALID PRINT LEVEL RECEIVED: " + configurationMap[constants::KEY_PRINT_LEVEL] + "\n");
    } else {
        _printLevel = utils::contains(configurationMap, constants::KEY_PRINT_LEVEL) ?
            static_cast<constants::PrintLevel>(std::atoi(configurationMap[constants::KEY_PRINT_LEVEL].c_str())) : constants::DEFAULT_PRINT_LEVEL;
    }

    GlobalSearchAlgorithmParams globalSearchAlgParams(reliability, accuracy);
    IndexAlgorithmParams indexAlgParams(reliability, accuracy, epsilonReserved, iterationLimit);
    ScanParams scanParams(densityScan, keyScan);
    if (utils::contains(configurationMap, constants::KEY_CUSTOM_TASK)) {
        std::string taskId = configurationMap[constants::KEY_CUSTOM_TASK];
        _algorithmsMap[taskId] = createAlgorithm(algType,
            parser::parseCustomTask(constants::API_DIR, constants::CONFIG_PATH_FILE,
                taskId + constants::TXT_FILE_EXTENSION), globalSearchAlgParams, indexAlgParams, scanParams);
    }
    else {
        _constrainedProblemFamily = new TGrishaginConstrainedProblemFamily();
        int startTaskId, endTaskId;
        if (utils::contains(configurationMap, constants::KEY_TASK_NUMBER)) {
            startTaskId = std::atoi(configurationMap[constants::KEY_TASK_NUMBER].c_str());
            endTaskId = startTaskId + 1;
        }
        else {
            startTaskId = 0;
            endTaskId = _constrainedProblemFamily->GetFamilySize();
        }

        for (int i = startTaskId; i < endTaskId; i++) {
            _algorithmsMap[std::to_string(i)] = createAlgorithm(algType, TemplateTask(_constrainedProblemFamily->operator[](i),
                TrialPoint(_constrainedProblemFamily->operator[](i)->GetOptimumPoint(), _constrainedProblemFamily->operator[](i)->GetOptimumValue())),
                globalSearchAlgParams, indexAlgParams, scanParams);
        }
    }

    std::vector<std::string> paths = parser::parseDirectories(constants::API_DIR, constants::DATA_PATHS_FILE);
    if (paths.size() != constants::DATA_PATHS_COUNT) {
        _logger->log("CONFIGURATION CANNOT BE COMPLETED.\n");
        throw ErrorWrapper(Errors::CONFIGURATION_ERROR, "[CONFIGURATOR] API (DATA PATHS FILE) IS CHANGED.\n");
    }
    _algorithmPointsDir = paths[0];
    _functionPointsDir = paths[1];
    _invalidPointsDir = paths[2];

    _logger->log("CONFIGURATION SUCCESSFULLY COMPLETED.\n");
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
        return new GlobalSearchAlgorithm(templateTask, globalSearchAlgParams, scanParams);
    }
    else {
        _logger->log("CONFIGURATION COMPLETED WITH ERROR.\n");
        throw ErrorWrapper(Errors::CONFIGURATION_ERROR, "[CONFIGURATOR] UNKNOWN TASK TYPE: " + algType + "\n");
    }
}

void AlgorithmConfigurator::run() {
    PointType maxDeviation = -DBL_MAX;
    for (const auto& [taskId, algorithm] : _algorithmsMap) {
        _logger->log("CALCULATION OPTIMUM TASK <" + taskId + "> SUCCESSFULLY STARTED.\n");
        _logger->log("EXPECTED OPTIMUM: " +
            getPointDescription(_algorithmsMap[taskId]->getTask().getOptimumPoint(),
                _algorithmsMap[taskId]->getTask().getOptimumValue()) + "\n");
        auto result = algorithm->run();
        if (result.has_value()) {
            maxDeviation = std::max(maxDeviation, utils::getMaxCoordinateDifference(result.value().point, _algorithmsMap[taskId]->getTask().getOptimumPoint()));
            _logger->log("FOUND OPTIMUM: " +
                getPointDescription(result.value().point, result.value().value) + "\n");
            _logger->log("MAX DEVIATION: " +
                std::to_string(utils::getMaxCoordinateDifference(result.value().point, _algorithmsMap[taskId]->getTask().getOptimumPoint())) + "\n");
        }
        else {
            _logger->log("ALGORITHM DID NOT FIND ANY VALID POINTS. TRY INCREASING ACCURACY, RELIABILITY PARAMETER OR SCAN DENSITY.\n");
        }
        _logger->log("ITERATION COUNT: " + std::to_string(algorithm->getComplexity().getIterationCount()) + "\n");
        _logger->log("CALCULATION COUNT: " +
            getCalculationCountDescription(algorithm->getComplexity().getFunctionsCalculationCount()) + "\n");
        _logger->log("\n");

        printPointsToFile(taskId, algorithm->getPoints());
    }

    if (!utils::equal(maxDeviation, -DBL_MAX)) {
        _logger->log("MAX DEVIATION AMONG ALL TASKS: " + std::to_string(maxDeviation) + "\n");
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
    for (int i = 0; i < calculationCounts.size(); i++) {
        calculationCount += std::to_string(calculationCounts[i]);
        if (i != calculationCounts.size() - 1) {
            calculationCount += ", ";
        }
    }
    return calculationCount;
}

void AlgorithmConfigurator::printPointsToFile(const std::string& taskId, Points points) {
    Borders borders = _algorithmsMap[taskId]->getTask().getTaskBorders();
    std::string pointsFileName = parser::parseFileName(constants::API_DIR, constants::NAME_CONTRACT_FILE, taskId);
    switch (_printLevel) {
    case constants::PrintLevel::PRINT_ALL_POINTS:
        writer::writePointIntervalToFile(_functionPointsDir + pointsFileName, borders.leftBorder, borders.rightBorder,
            constants::STEP_PRINT_POINTS, [&](Point point) { return _algorithmsMap[taskId]->getTask().getTaskValue(point); },
            [&](Point point) { return true; });
        writer::writePointIntervalToFile(_invalidPointsDir + pointsFileName, borders.leftBorder, borders.rightBorder,
            constants::STEP_PRINT_POINTS, [&](Point point) { return _algorithmsMap[taskId]->getTask().getTaskValue(point); },
            [&](Point point) {
                bool isValid = true;
                for (int i = 0; i < _algorithmsMap[taskId]->getTask().getConstraintsCount(); i++) {
                    if (_algorithmsMap[taskId]->getTask().getConstraintValue(i, point) > 0) {
                        isValid = false;
                        break;
                    }
                }
                return !isValid;
            });
    case constants::PrintLevel::PRINT_ONLY_TRIAL_POINT:
        points.push_back(_algorithmsMap[taskId]->getTask().getOptimumPoint());
        writer::writePointsToFile(_algorithmPointsDir + pointsFileName,
            points, [&](Point point) { return _algorithmsMap[taskId]->getTask().getTaskValue(point); });
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
