#pragma once

#include "index_algorithm.hpp"


IndexAlgorithm::IndexAlgorithm(const Function& task, const std::vector<Function>& constraints, const IndexAlgorithmParams& params) :
    _taskHelper(task, constraints), _params(params), _points(), _complexity(),
	_peanoPoints(), _peanoPointsClassification(), _performedStepsMap(), _isNeededStop(false),
	_estimationLipschitzConstant(constraints.size() + 1, -DBL_MAX), _minZs(constraints.size() + 1, DBL_MAX) {}

Point IndexAlgorithm::parsePoint(PointType peanoPoint) {
	if (_taskHelper.getTask().getDimensionSize() > 1) {
		if (peanoPoint < constants::MIN_PEANO_POINT || peanoPoint > constants::MAX_PEANO_POINT) {
			throw errors::INDEX_ALGORITHM_INTERNAL_ERROR_ERR_CODE;
		}

		double* y = new double[_taskHelper.getTask().getDimensionSize()];
		utils::mapd(peanoPoint, constants::DEFAULT_MAPD_M, y, static_cast<int>(_taskHelper.getTask().getDimensionSize()), constants::DEFAULT_MAPD_KEY);

		std::vector<PointType> nonLinearizedPoint(y, y + _taskHelper.getTask().getDimensionSize());
		auto linearArg = utils::linearTransform(nonLinearizedPoint, _taskHelper.getTask().getLeftBorder(), _taskHelper.getTask().getRightBorder());

		delete[] y;

		return linearArg;
	} else {
		return { peanoPoint };
	}
}

PointType IndexAlgorithm::startIteration() {
	_peanoPoints.insert(constants::MIN_PEANO_POINT);
	_peanoPoints.insert(constants::MAX_PEANO_POINT);

	std::string performedStepKey = performStep(constants::MIN_PEANO_POINT);
	updateData(performedStepKey, constants::MIN_PEANO_POINT);

	performedStepKey = performStep(constants::MAX_PEANO_POINT);
	updateData(performedStepKey, constants::MAX_PEANO_POINT);

	auto marks = calculateMarks();

	return calculateNextStepPeanoPoint(marks);
}

std::string IndexAlgorithm::performStep(PointType peanoPoint) {
	Point point = parsePoint(peanoPoint);
	std::string stepKey = std::to_string(peanoPoint);

	std::size_t v = _taskHelper.getConstraintsCount();
	PointType z = -1;
	for (std::size_t i = 0; i < v; i++) {
		z = _taskHelper.getConstraint(i).getValue(point);
		if (z > 0) {
			v = i;
			break;
		}
	}

	if (z <= 0) {
		z = _taskHelper.getTask().getValue(point);
	}

	_performedStepsMap[stepKey] = IndexAlgorithmStepResult(v, z);
	return stepKey;
}

void IndexAlgorithm::updateData(const std::string& performedStepKey, PointType performedStepPeanoPoint) {
	IndexAlgorithmStepResult performedStepResult = _performedStepsMap[performedStepKey];

	std::string stepKey;
	for (auto peanoPoint: _peanoPointsClassification[performedStepResult.v]) {
		stepKey = std::to_string(peanoPoint);
		_estimationLipschitzConstant[performedStepResult.v] = std::max(_estimationLipschitzConstant[performedStepResult.v],
			fabs(performedStepResult.z - _performedStepsMap[stepKey].z) /
				powl(fabs(performedStepPeanoPoint - peanoPoint), 1.0 / _taskHelper.getTask().getDimensionSize()));
	}

	if (_estimationLipschitzConstant[performedStepResult.v] <= 0.0 || _peanoPointsClassification[performedStepResult.v].empty()) {
		_estimationLipschitzConstant[performedStepResult.v] = 1.0;
	}

	_peanoPointsClassification[performedStepResult.v].insert(performedStepPeanoPoint);
	_minZs[performedStepResult.v] = std::min(_minZs[performedStepResult.v], performedStepResult.z);
}

std::vector<long double> IndexAlgorithm::calculateMarks() {
	std::vector<long double> marks(_taskHelper.getConstraintsCount() + 1);
	for (std::size_t v = 0; v < marks.size(); v++) {
		if (!_peanoPointsClassification[v].empty()) {
			if (_minZs[v] > 0 || v == _taskHelper.getConstraintsCount()) {
				marks[v] = _minZs[v];
			}
			else {
				marks[v] = -_estimationLipschitzConstant[v] * _params.delta;
			}
		}
	}

	return marks;
}

PointType IndexAlgorithm::calculateNextStepPeanoPoint(const std::vector<long double>& marks) {
	std::pair<PointType, PointType> nextStepInterval;
	long double maxR = -DBL_MAX;

	PointType previousPoint, currentPoint;
	IndexAlgorithmStepResult previousPointStepResult, currentPointStepResult;
	long double delta, R;
	auto previousPointIter = _peanoPoints.begin();
	for (auto currentPointIter = ++_peanoPoints.begin(); currentPointIter != _peanoPoints.end(); currentPointIter++) {
		previousPoint = *previousPointIter;
		currentPoint = *currentPointIter;

		previousPointStepResult = _performedStepsMap[std::to_string(previousPoint)];
		currentPointStepResult = _performedStepsMap[std::to_string(currentPoint)];

		delta = powl(currentPoint - previousPoint, 1.0 / _taskHelper.getTask().getDimensionSize());

		if (previousPointStepResult.v == currentPointStepResult.v) {
			R = delta + pow((currentPointStepResult.z - previousPointStepResult.z), 2) /
				(pow(_params.rCoeff, 2) * pow(_estimationLipschitzConstant[currentPointStepResult.v], 2) * delta) -
				2.0 * (currentPointStepResult.z + previousPointStepResult.z - 2.0 * marks[currentPointStepResult.v]) /
				(_estimationLipschitzConstant[currentPointStepResult.v] * _params.rCoeff);
		} else if (currentPointStepResult.v > previousPointStepResult.v) {
			R = 2.0 * delta - 4.0 * (currentPointStepResult.z - marks[currentPointStepResult.v]) /
				(_estimationLipschitzConstant[currentPointStepResult.v] * _params.rCoeff);
		} else {
			R = 2.0 * delta - 4.0 * (previousPointStepResult.z - marks[previousPointStepResult.v]) /
				(_estimationLipschitzConstant[previousPointStepResult.v] * _params.rCoeff);
		}
		
		if (R > maxR) {
			maxR = R;
			nextStepInterval = { previousPoint, currentPoint };
		}

		previousPointIter = currentPointIter;
	}

	_isNeededStop = pow(nextStepInterval.second - nextStepInterval.first, 1.0 / _taskHelper.getTask().getDimensionSize()) <= _params.accuracy;

	std::string startIntervalPointKey = std::to_string(nextStepInterval.first);
	std::string endIntervalPointKey = std::to_string(nextStepInterval.second);
	if (_performedStepsMap[startIntervalPointKey].v != _performedStepsMap[endIntervalPointKey].v) {
		return (nextStepInterval.first + nextStepInterval.second) / 2.0;
	} else {
		return (nextStepInterval.first + nextStepInterval.second) / 2.0 - 
			utils::sign(_performedStepsMap[endIntervalPointKey].z - _performedStepsMap[startIntervalPointKey].z) *
			(1.0 / (2.0 * _params.rCoeff)) * pow(fabsl(_performedStepsMap[endIntervalPointKey].z - _performedStepsMap[startIntervalPointKey].z) /
				_estimationLipschitzConstant[_performedStepsMap[startIntervalPointKey].v], _taskHelper.getTask().getDimensionSize());
	}
}

TrialPoint IndexAlgorithm::run() {
	clearData();

	auto newPoint = startIteration();
	std::string performedStepKey;
	std::vector<long double> marks;
	while (!_isNeededStop) {
		_peanoPoints.insert(newPoint);
		performedStepKey = performStep(newPoint);
		updateData(performedStepKey, newPoint);
		marks = calculateMarks();

		newPoint = calculateNextStepPeanoPoint(marks);
		_points.push_back(parsePoint(newPoint));

		_complexity.iterationCount++;
	}

	auto arguments = parsePoint(newPoint);
	_points.push_back(arguments);
	auto value = _taskHelper.getTask().getValue(arguments);

	return TrialPoint(arguments, value);
}

void IndexAlgorithm::clearData() {
	_points.clear();
	_complexity = Complexity();
	_peanoPoints.clear();
	_peanoPointsClassification.clear();
	_performedStepsMap.clear();
	_isNeededStop = false;
	_estimationLipschitzConstant = std::vector<long double>(_taskHelper.getConstraintsCount() + 1, -DBL_MAX);
	_minZs = std::vector<PointType>(_taskHelper.getConstraintsCount() + 1, DBL_MAX);
}

Points IndexAlgorithm::getPoints() {
	return _points;
}

Complexity IndexAlgorithm::getComplexity() {
	return _complexity;
}