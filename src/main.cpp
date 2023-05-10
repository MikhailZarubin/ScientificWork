#include "algorithm_configurator.hpp"
#include <iostream>
#include <fstream>


int main(int argc, char* argv[]) {
    try {
        std::ofstream outputFile("log.txt");
        outputFile.clear();
        AlgorithmConfigurator algorithmConfigurator(argc, argv, [&](const std::string& log) {
            std::cout << log;
            outputFile << log;
            }
        );
        algorithmConfigurator.run();
        outputFile.close();
    }
    catch (...) {
        // stub while not implemented internal exception handling
        std::cout << "Something went wrong\n";
    }
}