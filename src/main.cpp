#include "algorithm_configurator.hpp"
#include "logger.hpp"


int main(int argc, char* argv[]) {
    Logger logger;
    std::string errorMessage;
    int returnedCode = constants::GOOD_RETURN_CODE;
    try {
        logger.init(constants::LOG_FILE);
        AlgorithmConfigurator algorithmConfigurator(argc, argv, &logger);
        algorithmConfigurator.run();
        logger.close();
    }
    catch (const ErrorWrapper& internalError) {
        errorMessage = "INTERNAL ERROR: " + internalError.getErrorMessage();
        if (logger.isReady()) {
            logger.log(errorMessage);
        }
        else {
            std::cerr << errorMessage;
        }
        returnedCode = internalError.getErrorCode();
    }
    catch (const std::runtime_error& runtimeError) {
        errorMessage = "RUNTIME ERROR: " + std::string(runtimeError.what()) + "\n";
        if (logger.isReady()) {
            logger.log(errorMessage);
        }
        else {
            std::cerr << errorMessage;
        }
        returnedCode = constants::BAD_RETURN_CODE;
    }
    catch (const std::exception& exception) {
        errorMessage = "ERROR OCCURED: " + std::string(exception.what()) + "\n";
        if (logger.isReady()) {
            logger.log(errorMessage);
        }
        else {
            std::cerr << errorMessage;
        }
        returnedCode = constants::BAD_RETURN_CODE;
    }
    catch (...) {
        errorMessage = "UNKNOWN ERROR OCCURRED. POSSIBLE MEMORY CORRUPTION.\n";
        if (logger.isReady()) {
            logger.log(errorMessage);
        }
        else {
            std::cerr << errorMessage;
        }
        returnedCode = constants::BAD_RETURN_CODE;
    }
    return returnedCode;
}
