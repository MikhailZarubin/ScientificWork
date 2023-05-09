#pragma once

#include "base_algorithm.hpp"
#include "complexity.hpp"
#include "constants.hpp"
#include "constrained_task_helper.hpp"
#include "errors.hpp"
#include "function.hpp"
#include "point_set_model_helper.hpp"
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <string>


class IndexAlgorithm : public Algorithm {
protected:
	ConstrainedTaskHelper _taskHelper;
	IndexAlgorithmParams _algParams;
	ScanParams _scanParams;
	Points _points;
	Complexity _complexity;

	std::optional<Point> _optimalPoint;
	std::optional<PointType> _optimalValue;

	std::set<PointType> _peanoPoints;
	std::map<std::size_t, IndexAlgorithmPointSetModel> _peanoPointsClassification;
	std::map<std::string, IndexAlgorithmStepResult> _performedStepsMap;
	std::vector<long double> _estimationLipschitzConstant;
	std::vector<PointType> _minZs;
	std::size_t maxV;

	Point parsePoint(PointType peanoPoint);
	PointType startIteration();
	virtual std::string performStep(PointType peanoPoint);
	void updateData();
	std::vector<long double> calculateMarks();
	virtual std::pair<PointType, PointType> calculateNextStepInterval(const std::vector<long double>& marks);
	virtual long double calculateInterval—haracteristic(long double delta, const std::vector<long double>& marks,
		IndexAlgorithmStepResult previousPointStepResult, IndexAlgorithmStepResult currentPointStepResult);
	virtual PointType calculateNextStepPeanoPoint(std::pair<PointType, PointType> nextStepInterval);

	void updateOptimalPoint(Point potentialOptimalPoint, PointType potentialOptimalValue,
		std::optional<bool> isValid = std::optional<bool>());
	void clearData();

public:
	IndexAlgorithm() = delete;
	IndexAlgorithm(const Function& task, const std::vector<Function>& constraints, 
		const IndexAlgorithmParams& algParams, const ScanParams& scanParams);
	IndexAlgorithm(IConstrainedOptProblem* generator, 
		const IndexAlgorithmParams& params, const ScanParams& scanParams);
	Points getPoints();
	Complexity getComplexity();
	TrialPoint run();
};

