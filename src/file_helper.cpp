#pragma once

#include "file_helper.hpp"


Points deepEnumeration(Point truncatedStartPoint, Point finishPoint, PointType step) {
	Points points;
	PointType currentValue = truncatedStartPoint[0];
	PointType finishValue = finishPoint[finishPoint.size() - truncatedStartPoint.size()];
	if (truncatedStartPoint.size() == 1) {
		while (currentValue <= finishValue) {
			points.push_back({ currentValue });
			currentValue += step;
		}
	}
	else {
		Point newPoint(truncatedStartPoint.size());
		Point truncatedPoint(++truncatedStartPoint.begin(), truncatedStartPoint.end());
		while (currentValue <= finishValue) {
			newPoint[0] = currentValue;
			for (Point reciviedPoint : deepEnumeration(truncatedPoint, finishPoint, step)) {
				std::copy(reciviedPoint.begin(), reciviedPoint.end(), newPoint.begin() + 1);
				points.push_back(newPoint);
			}
			currentValue += step;
		}
	}
	return points;
}

std::vector<std::string> parser::parseDirectories(const std::string& pathsFilePath, const std::string& pathsFileName) {
	std::ifstream pathsFile(pathsFilePath + pathsFileName);
	std::string path;
	std::vector<std::string> paths;

	while (std::getline(pathsFile, path)) {
		paths.push_back(path);
	}
	return paths;
}

std::string parser::parseFileName(const std::string& nameContractFilePath, const std::string& nameContractFileName, int taskNumber) {
	std::ifstream nameContractFile(nameContractFilePath + nameContractFileName);
	std::string nameContract;

	std::getline(nameContractFile, nameContract);
	std::vector<std::string> partsName = utils::split(nameContract, "{}");
	return partsName[0] + std::to_string(taskNumber) + partsName[1];
}

void writer::writePointsToFile(const std::string& fileName, Points points, const std::function<PointType(Point)>& getValue) {
	std::ofstream outputFile(fileName);
	outputFile.clear();

	for (Point point : points) {
		for (PointType value : point) {
			outputFile << value << ' ';
		}
		outputFile << getValue(point) << '\n';
	}

	outputFile.close();
}

void writer::writePointIntervalToFile(const std::string& fileName, Point startPoint, Point finishPoint, PointType step,
	const std::function<PointType(Point)>& getValue, const std::function<bool(Point)>& condition) {
	if (startPoint.size() != finishPoint.size()) {
		throw errors::FILE_WRITER_ERROR_ERR_CODE;
	}

	std::ofstream outputFile(fileName);
	outputFile.clear();

	Points allPoints = deepEnumeration(startPoint, finishPoint, step);
	for (Point point : allPoints) {
		if (condition(point)) {
			for (PointType value : point) {
				outputFile << value << ' ';
			}
			outputFile << getValue(point) << '\n';
		}
	}

	outputFile.close();
}