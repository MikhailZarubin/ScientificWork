#pragma once

#include "constants.hpp"
#include "errors.hpp"
#include <optional>
#include <vector>
#include <set>

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

struct GlobalSearchAlgorithmParams {
    double rCoeff;
    double accuracy;

    GlobalSearchAlgorithmParams(double rCoeff, double accuracy);
    GlobalSearchAlgorithmParams(const GlobalSearchAlgorithmParams& params);
};

struct IndexAlgorithmParams {
    double rCoeff;
    double accuracy;
    double delta;

    IndexAlgorithmParams(double rCoeff, double accuracy, double delta);
    IndexAlgorithmParams(const IndexAlgorithmParams& params);
};

struct ScanParams {
    int density;
    int key;

    ScanParams(int density, int key);
    ScanParams(const ScanParams& params);
};

struct Borders {
    Point leftBorder;
    Point rightBorder;

    Borders(const Point& leftBorder, const Point& rightBorder);
    Borders(const Borders& borders);
};

struct IndexAlgorithmStepResult {
    PointType point;
    std::size_t v;
    PointType z;

    IndexAlgorithmStepResult(PointType point = 0, std::size_t v = 0, PointType z = 0);
    IndexAlgorithmStepResult(const IndexAlgorithmStepResult& indexAlgorithmStepResult);
};

struct IndexAlgorithmPointSetModel {
    std::set<PointType> reviewedPoints;
    std::optional<PointType> newPoint;

    IndexAlgorithmPointSetModel(std::set<PointType> reviewedPoints = {}, std::optional<PointType> newPoint = std::optional<PointType>());
    IndexAlgorithmPointSetModel(const IndexAlgorithmPointSetModel& indexAlgorithmPointSetModel);
};