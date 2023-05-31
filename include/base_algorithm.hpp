#pragma once

#include "complexity.hpp"
#include "models.hpp"
#include "template_task.hpp"


class Algorithm {
public:
    virtual Points getPoints() = 0;
    virtual Complexity getComplexity() = 0;
    virtual TemplateTask getTask() = 0;
    virtual TrialPoint run() = 0;
};