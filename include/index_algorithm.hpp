#pragma once

#include "base_algorithm.hpp"
#include "complexity.hpp"
#include "constants.hpp"
#include "constrained_task_helper.hpp"
#include "errors.hpp"
#include "function.hpp"
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <string>


class IndexAlgorithm : public Algorithm {
protected:
	ConstrainedTaskHelper _taskHelper;
	IndexAlgorithmParams _params;
	Points _points;
	Complexity _complexity;

	std::set<PointType> _peanoPoints;
	std::map<std::size_t, std::set<PointType>> _peanoPointsClassification;
	std::map<std::string, IndexAlgorithmStepResult> _performedStepsMap;
	std::vector<long double> _estimationLipschitzConstant;
	std::vector<PointType> _minZs;
	std::size_t maxV;

	Point parsePoint(PointType peanoPoint);
	PointType startIteration();
	virtual std::string performStep(PointType peanoPoint);
	void updateData(const std::string& performedStepKey, PointType performedStepPoint);
	std::vector<long double> calculateMarks();
	virtual std::pair<PointType, PointType> calculateNextStepInterval(const std::vector<long double>& marks);
	PointType calculateNextStepPeanoPoint(std::pair<PointType, PointType> nextStepInterval);

	void clearData();

public:
	IndexAlgorithm() = delete;
	IndexAlgorithm(const Function& task, const std::vector<Function>& constraints, const IndexAlgorithmParams& params);
	IndexAlgorithm(IConstrainedOptProblem* generator, const IndexAlgorithmParams& params);
	Points getPoints();
	Complexity getComplexity();
	TrialPoint run();
};

