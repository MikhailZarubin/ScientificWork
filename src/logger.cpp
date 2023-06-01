#pragma once

#include "logger.hpp"


void Logger::init(const std::string& logFileName) {
    if (_logFile.is_open()) {
        throw ErrorWrapper(Errors::LOGGER_ERROR, "[LOGGER] LOGGER HAS BEEN ALREADY INITIALIZED.\n");
    }

    _logFile.open(logFileName);
    _logFile.clear();
}

bool Logger::isReady() {
    return _logFile.is_open();
}

void Logger::log (const std::string& message) {
    std::cout << message;
    _logFile << message;
}

void Logger::close() {
    if (!_logFile.is_open()) {
        throw ErrorWrapper(Errors::LOGGER_ERROR, "[LOGGER] LOGGER IS NOT INITIALIZED.\n");
    }

    _logFile.close();
}

Logger::~Logger() {
    if (_logFile.is_open()) {
        _logFile.close();
    }
}
