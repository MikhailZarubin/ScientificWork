#pragma once

#include "index_algorithm.hpp"
#include <numeric>


enum class OrderCheckingMode {
    BASE_IMPROVEMENT_MODE
};

class ModifiedIndexAlgorithm : public IndexAlgorithm {
    OrderCheckingMode _mode;
    std::vector<std::size_t> _orderChecking;

    std::string performStep(PointType peanoPoint) override;
    std::pair<PointType, PointType> calculateNextStepInterval(const std::vector<long double>& marks) override;
    void reorderCheckCostraints(std::size_t numberBrokenConstraint);
    
public:
    ModifiedIndexAlgorithm() = delete;
    ModifiedIndexAlgorithm(const Function& task, const std::vector<Function>& constraints, const IndexAlgorithmParams& params,
        const OrderCheckingMode& orderCheckingMode = OrderCheckingMode::BASE_IMPROVEMENT_MODE);
    ModifiedIndexAlgorithm(IConstrainedOptProblem* generator, const IndexAlgorithmParams& params,
        const OrderCheckingMode& orderCheckingMode = OrderCheckingMode::BASE_IMPROVEMENT_MODE);
};

