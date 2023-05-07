#pragma once

#include "index_algorithm.hpp"
#include <numeric>


class ModifiedIndexAlgorithm : public IndexAlgorithm {
    std::vector<std::size_t> _orderCheckingConstraintsNewPoint;
    std::map<string, std::vector<std::size_t>> _orderCheckingContraintsByPoint;

    std::string performStep(PointType peanoPoint) override;
    std::pair<PointType, PointType> calculateNextStepInterval(const std::vector<long double>& marks) override;
    long double calculateInterval—haracteristic(long double delta, const std::vector<long double>& marks,
        IndexAlgorithmStepResult previousPointStepResult, IndexAlgorithmStepResult currentPointStepResult) override;
    PointType calculateNextStepPeanoPoint(std::pair<PointType, PointType> nextStepInterval) override;

    void init();
    void reorderCheckingCostraints(const std::string& startIntervalKey, const std::string& endIntervalKey);
    std::size_t getConstraintOrdinalNumber(std::vector<std::size_t> constraintsOrder, std::size_t constraintIndex);
    
public:
    ModifiedIndexAlgorithm() = delete;
    ModifiedIndexAlgorithm(const Function& task, const std::vector<Function>& constraints,
        const IndexAlgorithmParams& algParams, const ScanParams& scanParams);
    ModifiedIndexAlgorithm(IConstrainedOptProblem* generator,
        const IndexAlgorithmParams& algParams, const ScanParams& scanParams);
};

