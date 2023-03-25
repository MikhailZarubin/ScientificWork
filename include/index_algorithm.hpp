#pragma once

#include "base_algorithm.hpp"
#include "constants.hpp"
#include "constrained_task_helper.hpp"
#include "errors.hpp"
#include "function.hpp"
#include <map>
#include <set>
#include <string>


class IndexAlgorithm : public Algorithm {
	ConstrainedTaskHelper _taskHelper;
	IndexAlgorithmParams _params;
	Points _points;
	Complexity _complexity;

	std::set<PointType> _peanoPoints;
	std::map<std::size_t, std::set<PointType>> _peanoPointsClassification;
	std::map<std::string, IndexAlgorithmStepResult> _performedStepsMap;
	std::vector<long double> _estimationLipschitzConstant;
	std::vector<PointType> _minZs;
	bool _isNeededStop;

	Point parsePoint(PointType peanoPoint);
	PointType startIteration();
	std::string performStep(PointType peanoPoint);
	void updateData(const std::string& performedStepKey, PointType performedStepPoint);
	std::vector<long double> calculateMarks();
	PointType calculateNextStepPeanoPoint(const std::vector<long double>& marks);

	void clearData();

public:
	IndexAlgorithm() = delete;
	IndexAlgorithm(const Function& task, const std::vector<Function>& constraints, const IndexAlgorithmParams& params);
	Points getPoints();
	Complexity getComplexity();
	TrialPoint run();
};

