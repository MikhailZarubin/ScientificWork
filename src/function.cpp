#pragma once

#include "function.hpp"


Function::Function(const std::string& expression, const std::string& variableSet, std::optional<Borders> borders) :
	_expression(expression), _variableSet(variableSet), _borders(borders) {}

PointType Function::getValue(const Point& point) {
	if (_borders.has_value() && (_borders.value().leftBorder > point || _borders.value().rightBorder < point)) {
		throw errors::POINT_OUT_OF_RANGE_ERR_CODE;
	}

	std::string expressionWithValue = _expression;
	std::string::size_type pos = std::string::npos;
	bool isFirstReplace = true;
	for (std::string::size_type i = 0; i < _variableSet.size(); i++) {
		isFirstReplace = true;
		do {
			pos = expressionWithValue.find(_variableSet[i]);
			std::string valuePoint = "(" + std::to_string(point[static_cast<std::vector<long double>::size_type>(i)]) + ")";

			if (pos != std::string::npos) {
				expressionWithValue.replace(pos, 1, valuePoint);
			}
			else if (isFirstReplace) {
				throw errors::INCORRECT_VARIABLE_SET_ERR_CODE;
			}

			isFirstReplace = false;
			
		} while (pos != std::string::npos);
	}

	return Calculator(expressionWithValue).calculateResult();
}

Point Function::getLeftBorder() {
	if (!_borders.has_value()) {
		throw errors::BORDERS_NOT_FOUND_ERR_CODE;
	}
	return _borders.value().leftBorder;
}

Point Function::getRightBorder() {
	if (!_borders.has_value()) {
		throw errors::BORDERS_NOT_FOUND_ERR_CODE;
	}
	return _borders.value().rightBorder;
}
