#pragma once

#include <map>
#include <optional>


enum class Operation {
    SUM = '+',
    SUB = '-',
    MUL = '*',
    DIV = '/',
    SIN = 'S',
    COS = 'C',
    EXP = 'E',
    LOG = 'L',
    POW = '^'
};

enum class Priority {
    DELIMITER_PRIORITY = -1,
    NUMBER_PRIORITY = 0,
    BRACKETS_PRIORITY = 1,
    SECONDARY_OPERATIONS_PRIPRITY = 2,
    PRIMARY_OPERATIONS_PRIORITY = 3,
    IMMEDIATELY_OPERATIONS_PRIORITY = 4
};

enum class AssistiveSymbols {
    OPENING_BRACKET = '(',
    CLOSING_BRACKET = ')',
    DELIMITER = '|',
    EXPONENT_INDICATOR = 'e'
};

class OperationsHelper {
    std::map<char, Operation> _operationMap;
    std::map<char, AssistiveSymbols> _assistiveSymbolsMap;

public:
    OperationsHelper();
    bool isOperation(char sybmol) const;
    std::optional<Operation> getOperationBySymbol(char symbol) const;
    Priority getPriority(char possibleOperation) const;
};