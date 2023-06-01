#pragma once

#include<string>


enum class Errors {
    CONFIGURATION_ERROR = 1,
    CALCULATOR_ERROR = 2,
    COMPLEXITY_ERROR = 3,
    FILE_PARSER_ERROR = 4,
    FILE_WRITER_ERROR = 5,
    FUNCTION_ERROR = 6,
    POINT_SET_MODEL_HELPER_ERROR = 7,
    TEMPLATE_TASK_ERROR = 8,
    UTILS_ERROR = 9,
    MODELS_ERROR = 10,
    MODIFIED_INDEX_ALG_ERROR = 11
};

class ErrorWrapper {
    Errors _error;
    std::string _errorMessage;

public:
    ErrorWrapper(Errors error, const std::string& errorMessage);
    ErrorWrapper(const ErrorWrapper& errorWrapper);
    int getErrorCode() const;
    std::string getErrorMessage() const;
};