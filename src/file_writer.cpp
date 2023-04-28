#pragma once

#include "file_writer.hpp"


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

void writer::writePointsToFile(const std::string& fileName, Point startPoint, Point finishPoint, PointType step,
	const std::function<PointType(Point)> getValue) {
	std::ofstream outputFile(fileName);
	outputFile.clear();

	Points allPoints = deepEnumeration(startPoint, finishPoint, step);
	for (Point point : allPoints) {
		for (PointType value : point) {
			outputFile << value << ' ';
		}
		outputFile << getValue(point) << '\n';
	}

	outputFile.close();
}
