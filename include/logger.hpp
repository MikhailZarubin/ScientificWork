#pragma once

#include "constants.hpp"
#include "errors.hpp"
#include <iostream>
#include <fstream>


class Logger {
    std::ofstream _logFile;

public:
    Logger() = default;
    void init(const std::string& logFileName);
    bool isReady();
    void log(const std::string& message);
    void close();
    ~Logger();
};
