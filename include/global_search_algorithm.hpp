#pragma once

#include "base_algorithm.hpp"
#include "models.hpp"
#include "function.hpp"
#include <set>

class BidimensionalGlobalSearch : public Algorithm {
	Function _task;
	GlobalSearchTaskParams _params;

	TrialPoint _globalMinimum;
	std::set<TrialPoint, bool(*) (const TrialPoint&, const TrialPoint&)> _checkedPoints;
	Complexity _complexity;
	Points _points;

	void startIteration();
	void clearCache();

public:
	BidimensionalGlobalSearch() = delete;
	BidimensionalGlobalSearch(const Function& task, const GlobalSearchTaskParams& params);
	Points getPoints();
	Complexity getComplexity();
	TrialPoint run();
};

