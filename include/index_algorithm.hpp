#pragma once

#include "base_algorithm.hpp"
#include "constants.hpp"
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
	TemplateTask _task;
	IndexAlgorithmParams _algParams;
	ScanParams _scanParams;
	Points _points;
	Complexity _complexity;

	std::optional<Point> _optimalPoint;
	std::optional<PointType> _optimalValue;

	std::set<PointType> _mappedPoints;
	std::map<std::size_t, IndexAlgorithmPointSetModel> _mappedPointsClassification;
	std::map<std::string, IndexAlgorithmStepResult> _performedStepsMap;
	std::vector<long double> _estimationLipschitzConstant;
	std::vector<PointType> _minZs;
	std::size_t maxV;

	PointType startIteration();
	virtual std::string performStep(PointType mappedPoint);
	void updateData();
	std::vector<long double> calculateMarks();
	virtual std::pair<PointType, PointType> calculateNextStepInterval(const std::vector<long double>& marks);
	virtual long double calculateIntervalCharacteristic(long double delta, const std::vector<long double>& marks,
		IndexAlgorithmStepResult previousPointStepResult, IndexAlgorithmStepResult currentPointStepResult);
	virtual PointType calculateNextStepMappedPoint(std::pair<PointType, PointType> nextStepInterval);

	void updateOptimalPoint(Point potentialOptimalPoint, PointType potentialOptimalValue,
		std::optional<bool> isValid = std::optional<bool>());
	void clearData();

public:
	IndexAlgorithm() = delete;
	IndexAlgorithm(const TemplateTask& task, const IndexAlgorithmParams& algParams, const ScanParams& scanParams);
	Points getPoints();
	Complexity getComplexity();
	TemplateTask getTask();
	TrialPoint run();
};

