#pragma once

#include "constants.hpp"
#include "errors.hpp"
#include <vector>

typedef double PointType;
typedef std::vector<PointType> Point;
typedef std::vector<Point> Points;

struct TrialPoint {
    Point point;
    PointType value;

    TrialPoint(Point point = Point(), PointType value = PointType());
    TrialPoint(const TrialPoint& trialPoint);
    TrialPoint& operator = (const TrialPoint& trialPoint);
};

struct Complexity {
    long int iterationCount;

    Complexity(long int iterationCount = constants::DEF_COMPLEXITY);
    Complexity(const Complexity& complexity);
};

struct GlobalSearchTaskParams {
    double rCoeff;
    double accuracy;

    GlobalSearchTaskParams(double rCoeff, double accuracy);
    GlobalSearchTaskParams(const GlobalSearchTaskParams& params);
};

struct Borders {
    Point leftBorder;
    Point rightBorder;

    Borders(const Point& leftBorder, const Point& rightBorder);
    Borders(const Borders& borders);
};