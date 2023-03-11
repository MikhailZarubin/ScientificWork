#pragma once

#include "function.hpp"


Function::Function(const std::string& expression, const std::string& variableSet, Borders *borders) : _expression(expression), _variableSet(variableSet) {
	_borders = nullptr;
	if (borders != nullptr) {
		this->_borders = new Borders(*borders);
	}
}

PointType Function::getValue(const Point& point) {
	if (_borders != nullptr && (_borders->leftBorder > point || _borders->rightBorder < point)) {
		throw errors::POINT_OUT_OF_RANGE_ERR_CODE;
	}

	std::string expressionWithValue = _expression;

	for (std::string::size_type i = 0; i < _variableSet.size(); i++) {
		std::string::size_type pos = expressionWithValue.find(_variableSet[i]);
		if (pos != std::string::npos) {
			std::string valuePoint = "(" + std::to_string(point[static_cast<Point::size_type>(i)]) + ")";
			expressionWithValue.replace(pos, 1, valuePoint);
		}
	}

	return Calculator(expressionWithValue).calculateResult();
}

Point Function::getLeftBorder() {
	if (_borders == nullptr) {
		throw errors::BORDERS_NOT_FOUND_ERR_CODE;
	}
	return _borders->leftBorder;
}

Point Function::getRightBorder()
{
	if (_borders == nullptr) {
		throw errors::BORDERS_NOT_FOUND_ERR_CODE;
	}
	return _borders->rightBorder;
}
