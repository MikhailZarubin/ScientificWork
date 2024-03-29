#pragma once

#include "index_algorithm.hpp"
#include <numeric>


class ModifiedIndexAlgorithm : public IndexAlgorithm {
    std::vector<std::size_t> _orderCheckingConstraintsNewPoint;
    std::map<string, std::vector<std::size_t>> _orderCheckingContraintsByPoint;

    std::string performStep(PointType mappedPoint) override;
    std::pair<PointType, PointType> calculateNextStepInterval(const std::vector<long double>& marks) override;
    long double calculateIntervalCharacteristic(long double delta, const std::vector<long double>& marks,
        IndexAlgorithmStepResult previousPointStepResult, IndexAlgorithmStepResult currentPointStepResult) override;
    PointType calculateNextStepMappedPoint(std::pair<PointType, PointType> nextStepInterval) override;

    void reorderCheckingCostraints(const std::string& startIntervalKey, const std::string& endIntervalKey);
    std::size_t getConstraintOrdinalNumber(std::vector<std::size_t> constraintsOrder, std::size_t constraintIndex);
    
public:
    ModifiedIndexAlgorithm() = delete;
    ModifiedIndexAlgorithm(const TemplateTask& task, const IndexAlgorithmParams& algParams, const ScanParams& scanParams);
};

