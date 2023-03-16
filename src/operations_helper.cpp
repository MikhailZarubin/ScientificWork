#pragma once

#include "operations_helper.hpp"


OperationsHelper::OperationsHelper() : _operationMap(), _assistiveSymbolsMap() {
    _operationMap.insert(std::pair<char, Operation> (static_cast<char>(Operation::SUM), Operation::SUM));
    _operationMap.insert(std::pair<char, Operation>(static_cast<char>(Operation::SUB), Operation::SUB));
    _operationMap.insert(std::pair<char, Operation>(static_cast<char>(Operation::MUL), Operation::MUL));
    _operationMap.insert(std::pair<char, Operation>(static_cast<char>(Operation::DIV), Operation::DIV));
    _operationMap.insert(std::pair<char, Operation>(static_cast<char>(Operation::SIN), Operation::SIN));
    _operationMap.insert(std::pair<char, Operation>(static_cast<char>(Operation::COS), Operation::COS));
    _operationMap.insert(std::pair<char, Operation>(static_cast<char>(Operation::EXP), Operation::EXP));
    _operationMap.insert(std::pair<char, Operation>(static_cast<char>(Operation::LOG), Operation::LOG));
    _operationMap.insert(std::pair<char, Operation>(static_cast<char>(Operation::POW), Operation::POW));

    _assistiveSymbolsMap.insert(std::pair<char, AssistiveSymbols>(static_cast<char>(AssistiveSymbols::OPENING_BRACKET), AssistiveSymbols::OPENING_BRACKET));
    _assistiveSymbolsMap.insert(std::pair<char, AssistiveSymbols>(static_cast<char>(AssistiveSymbols::CLOSING_BRACKET), AssistiveSymbols::CLOSING_BRACKET));
    _assistiveSymbolsMap.insert(std::pair<char, AssistiveSymbols>(static_cast<char>(AssistiveSymbols::DELIMITER), AssistiveSymbols::DELIMITER));
}

bool OperationsHelper::isOperation(char symbol) const {
    return _operationMap.find(symbol) != _operationMap.end();
}

std::optional<Operation> OperationsHelper::getOperationBySymbol(char symbol) const {
    auto operation = _operationMap.find(symbol);
    if (operation == _operationMap.end()) {
        return std::optional<Operation>();
    } else {
        return std::optional<Operation>(operation->second);
    }
}

Priority OperationsHelper::getPriority(char symbol) const {
    Priority priority = Priority::NUMBER_PRIORITY;

    std::optional<Operation> possibleOperation = getOperationBySymbol(symbol);
    if (possibleOperation.has_value()) {
        switch (possibleOperation.value()) {
        case Operation::SIN:
        case Operation::COS:
        case Operation::EXP:
        case Operation::LOG:
            priority = Priority::BRACKETS_PRIORITY;
            break;
        case Operation::SUM:
        case Operation::SUB:
            priority = Priority::SECONDARY_OPERATIONS_PRIPRITY;
            break;
        case Operation::MUL:
        case Operation::DIV:
            priority = Priority::PRIMARY_OPERATIONS_PRIORITY;
            break;
        case Operation::POW:
            priority = Priority::IMMEDIATELY_OPERATIONS_PRIORITY;
            break;
        default:
            break;
        }
    }
    else {
        auto symbolIterator = _assistiveSymbolsMap.find(symbol);
        if (symbolIterator != _assistiveSymbolsMap.end()) {
            switch (symbolIterator->second) {
            case AssistiveSymbols::OPENING_BRACKET:
            case AssistiveSymbols::CLOSING_BRACKET:
                priority = Priority::BRACKETS_PRIORITY;
                break;
            case AssistiveSymbols::DELIMITER:
                priority = Priority::DELIMITER_PRIORITY;
                break;
            default:
                break;
            }
        }
    }

    return priority;
}