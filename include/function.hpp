#pragma once

#include "models.hpp"
#include "calculator.hpp"
#include <string>


class Function {
    std::string _expression;
    std::string _variableSet;
    Borders *_borders;

public:
    Function(const std::string& expression, const std::string& variableSet, Borders *borders = nullptr);
    Point getLeftBorder();
    Point getRightBorder();
    PointType getValue(const Point& point);
};