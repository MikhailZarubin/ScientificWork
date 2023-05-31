#pragma once

#include "base_algorithm.hpp"
#include "utils.hpp"
#include <set>


class GlobalSearchAlgorithm : public Algorithm {
	TemplateTask _task;
	GlobalSearchAlgorithmParams _algParams;
	ScanParams _scanParams;

	TrialPoint _optimumPoint;
	std::set<PointType> _checkedMappedPoints;
	std::map<std::string, PointType> _cachedFunctionValues;
	long double _maxAbsoluteFirstDifference;
	long double _algCoefficient;

	Complexity _complexity;
	Points _points;

	void startIteration();
	void updateAlgCoefficient(std::set<PointType>::iterator iterNewPoint);
	std::pair<PointType, PointType> calculateNextStepInterval();
	PointType calculateNextStepPoint(std::pair<PointType, PointType> nextStepInterval);
	void clearData();

public:
	GlobalSearchAlgorithm() = delete;
	GlobalSearchAlgorithm(const TemplateTask& task, const GlobalSearchAlgorithmParams& algParams, const ScanParams& scanParams);
	Points getPoints();
	Complexity getComplexity();
	TemplateTask getTask();
	TrialPoint run();
};

