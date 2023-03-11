#pragma once

#include"models.hpp"


class Algorithm {
public:
    virtual Points getPoints() = 0;
    virtual Complexity getComplexity() = 0;
    virtual TrialPoint run() = 0;
};