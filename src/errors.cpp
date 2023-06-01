#pragma once

#include "errors.hpp"


ErrorWrapper::ErrorWrapper(Errors error, const std::string& errorMessage) :
    _error(error), _errorMessage(errorMessage) {}

ErrorWrapper::ErrorWrapper(const ErrorWrapper& errorWrapper) :
    _error(errorWrapper._error), _errorMessage(errorWrapper._errorMessage) {}

int ErrorWrapper::getErrorCode() const {
    return static_cast<int>(_error);
}

std::string ErrorWrapper::getErrorMessage() const {
    return _errorMessage;
}