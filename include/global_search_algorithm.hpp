#pragma once

#include "base_algorithm.hpp"
#include <set>

class BidimensionalGlobalSearch : public Algorithm {
	TemplateTask _task;
	GlobalSearchAlgorithmParams _params;

	TrialPoint _globalMinimum;
	std::set<TrialPoint, bool(*) (const TrialPoint&, const TrialPoint&)> _checkedPoints;
	Complexity _complexity;
	Points _points;

	void startIteration();
	void clearData();

public:
	BidimensionalGlobalSearch() = delete;
	BidimensionalGlobalSearch(const TemplateTask& task, const GlobalSearchAlgorithmParams& params);
	Points getPoints();
	Complexity getComplexity();
	TemplateTask getTask();
	TrialPoint run();
};

