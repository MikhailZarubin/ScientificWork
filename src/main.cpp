#include "algorithm_configurator.hpp"
#include <iostream>
#include <fstream>


int main(int argc, char* argv[]) {
    std::ofstream logFile;
    std::function<void(const std::string&)> logger = [&](const std::string& log) {
        std::cout << log;
        logFile << log;
    };
    int returnedCode = constants::GOOD_RETURN_CODE;
    try {
        logFile.open(constants::LOG_FILE);
        logFile.clear();
        AlgorithmConfigurator algorithmConfigurator(argc, argv, logger);
        algorithmConfigurator.run();
    }
    catch (const ErrorWrapper& internalError) {
        logger("INTERNAL ERROR MESSAGE: " + internalError.getErrorMessage());
        returnedCode = internalError.getErrorCode();
    }
    catch (const std::runtime_error& runtimeError)
    {
        logger("RUNTIME ERROR: " + std::string(runtimeError.what()) + "\n");
    }
    catch (const std::exception& exception)
    {
        logger("ERROR OCCURED: " + std::string(exception.what()) + "\n");
    }
    catch (...)
    {
        logger("UNKNOWN ERROR OCCURRED. POSSIBLE MEMORY CORRUPTION.\n");
    }
    if (logFile.is_open()) {
        logFile.close();
    }
    return returnedCode;
}