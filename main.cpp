#include "algorithm_configurator.hpp"
#include "constants.hpp"
#include "file_writer.hpp"
#include <iostream>


int main(int argc, char* argv[]) {
    AlgorithmConfigurator algorithmConfigurator(argc, argv, [&](const std::string& log) { std::cout << log; });
    algorithmConfigurator.run();
    for (const auto& [taskNumber, points] : algorithmConfigurator.getPoints()) {
        int copyTaskNumber = taskNumber;
        writer::writePointsToFile(constants::pointFileName + std::to_string(copyTaskNumber) + constants::pointFileExtension,
            points, [&](Point point) { return algorithmConfigurator.getTask(copyTaskNumber)->ComputeFunction(point); });
    }
}