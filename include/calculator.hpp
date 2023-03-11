#pragma once

#include "models.hpp"
#include <string>
#include<stack>


class Calculator
{
	std::string infixExpression;
	std::string postfixExpression;

	int getPriority(char symbol) const;

	bool checkCorrectnessExpression() const;
	bool openingBrackets(char symbol) const;
	bool beginComplexFunction(char symbol) const;

	bool configureExpression(const std::string& basicExpression);
	void infixToPostfix();
public:
	Calculator() = delete;
	Calculator(const std::string& basicExpression);
	long double calculateResult();
};