#pragma once

#include "index_algorithm.hpp"
#include <numeric>


enum class ModificationMode {
    NONE,
    ADAPTIVE_ORDER_CHECKING,
    ADAPTIVE_CALCULATE_INTERVAL_CHARACTERISTIC
};

class ModifiedIndexAlgorithm : public IndexAlgorithm {
    ModificationMode _modificationMode;
    std::vector<std::size_t> _orderCheckingConstraintsNewPoint;
    std::map<string, std::vector<std::size_t>> _orderCheckingContraintsByPoint;

    std::string performStep(PointType peanoPoint) override;
    std::pair<PointType, PointType> calculateNextStepInterval(const std::vector<long double>& marks) override;
    long double calculateInterval—haracteristic(long double delta, const std::vector<long double>& marks,
        IndexAlgorithmStepResult previousPointStepResult, IndexAlgorithmStepResult currentPointStepResult) override;
    PointType calculateNextStepPeanoPoint(std::pair<PointType, PointType> nextStepInterval) override;

    void init();
    void reorderCheckingCostraints(std::size_t numberBrokenConstraint);
    std::size_t getConstraintOrderNumber(PointType point, std::size_t constraintIndex);
    
public:
    ModifiedIndexAlgorithm() = delete;
    ModifiedIndexAlgorithm(const Function& task, const std::vector<Function>& constraints, const IndexAlgorithmParams& params,
        const ModificationMode& orderCheckingMode = ModificationMode::NONE);
    ModifiedIndexAlgorithm(IConstrainedOptProblem* generator, const IndexAlgorithmParams& params,
        const ModificationMode& orderCheckingMode = ModificationMode::NONE);
};

