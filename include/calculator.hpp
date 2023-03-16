#pragma once

#include "models.hpp"
#include "operations_helper.hpp"
#include "utils.hpp"
#include <string>
#include <stack>


class Calculator {
	std::string _infixExpression;
	OperationsHelper _operationsHelper;
public:
	Calculator() = delete;
	Calculator(std::string basicExpression);
	long double calculateResult();
};