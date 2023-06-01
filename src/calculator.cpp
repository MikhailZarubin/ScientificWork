#pragma once

#include "calculator.hpp"

Calculator::Calculator(std::string basicExpression) : _operationsHelper() {
	_infixExpression = static_cast<char>(AssistiveSymbols::OPENING_BRACKET);
	basicExpression.erase(std::remove_if(basicExpression.begin(), basicExpression.end(),
		[](char symbol) { return std::isspace(symbol); }),
		basicExpression.end());
	for (std::string::size_type i = 0; i < basicExpression.size(); i++) {
		if (_operationsHelper.isOperation(basicExpression[i]) && _operationsHelper.getPriority(basicExpression[i]) == Priority::BRACKETS_PRIORITY) {
			_infixExpression += basicExpression[i];
			i += 3;

			if (i >= basicExpression.size() - 2) {
				throw ErrorWrapper(Errors::CALCULATOR_ERROR, "[CALCULATOR] EXPRESSION IS NOT ENDED OR UNKNOWN OPERATION IS SPECIFIED.\n");
			}
		} else {
			if (basicExpression[i] == '-' &&
				(_infixExpression.size() == 0 ||
				_operationsHelper.getPriority(_infixExpression[_infixExpression.size() - 1]) == Priority::BRACKETS_PRIORITY) &&
				_infixExpression[_infixExpression.size()-1] != static_cast<char>(AssistiveSymbols::CLOSING_BRACKET)) {
				_infixExpression += '0';
			}
			_infixExpression += basicExpression[i];
			if (basicExpression[i] == static_cast<char>(AssistiveSymbols::EXPONENT_INDICATOR)) {
				i++;
			}
		}
	}

	_infixExpression += static_cast<char>(AssistiveSymbols::CLOSING_BRACKET);

	if (!utils::isCorrectExpression(_operationsHelper, _infixExpression)) {
		throw ErrorWrapper(Errors::CALCULATOR_ERROR, "[CALCULATOR] EXPRESSION IS NOT CORRECTLY. PLEASE CHECK BRACKETS.\n");
	}
}

long double Calculator::calculateResult() {
	std::string postfixExpression = utils::infixToPostfix(_infixExpression);
	std::stack <long double> valuesStack;
	long double firstArg, secondArg;
	std::string number;
	for (std::string::size_type i = 0; i < postfixExpression.size(); i++) {
		if (_operationsHelper.isOperation(postfixExpression[i])) {
			if (_operationsHelper.getPriority(postfixExpression[i]) == Priority::BRACKETS_PRIORITY) {
				valuesStack.top() = utils::applyUnaryOperation(_operationsHelper.getOperationBySymbol(postfixExpression[i]).value(), valuesStack.top());
			} else {
				firstArg = valuesStack.top();
				valuesStack.pop();
				secondArg = valuesStack.top();
				valuesStack.top() = utils::applyBinaryOperation(_operationsHelper.getOperationBySymbol(postfixExpression[i]).value(), firstArg, secondArg);
			}

			if (valuesStack.top() == DBL_MIN) {
				throw ErrorWrapper(Errors::CALCULATOR_ERROR, "[CALCULATOR] UNKNOWN OPERATION BY INDEX " + std::to_string(i) + " IN EXPRESSION <" + postfixExpression + ">.\n");
			}	
		}
		else {
			while (i < postfixExpression.size() &&
				_operationsHelper.getPriority(postfixExpression[i]) == Priority::NUMBER_PRIORITY &&
				postfixExpression[i] != static_cast<char>(AssistiveSymbols::DELIMITER)) {
				number += postfixExpression[i];
				if (postfixExpression[i] == static_cast<char>(AssistiveSymbols::EXPONENT_INDICATOR)) {
					number += '-';
				}
				i++;
			}
			valuesStack.push(std::stold(number));
			number.clear();
		}
	}

	if (valuesStack.size() != 1) {
		throw ErrorWrapper(Errors::CALCULATOR_ERROR, "[CALCULATOR] CALCULATOR INTERNAL ERROR. PERHAPS THERE ARE TOO MANY OPERANDS IN EXPRESSION.\n");
	}

	return valuesStack.top();
}
