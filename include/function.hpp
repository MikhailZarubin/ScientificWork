#pragma once

#include "models.hpp"
#include "calculator.hpp"
#include <string>
#include <optional>


class Function {
    std::string _expression;
    std::string _variableSet;
    std::optional<Borders> _borders;

public:
    Function(const std::string& expression, const std::string& variableSet, std::optional<Borders> borders = std::optional<Borders>());
    Function(const Function& function);
    Point getLeftBorder();
    Point getRightBorder();
    PointType getValue(const Point& point);
    std::size_t getDimensionSize();
};