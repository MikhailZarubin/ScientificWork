#pragma once

#include "complexity.hpp"
#include "models.hpp"
#include "template_task.hpp"


class Algorithm {
protected:
    TemplateTask _task;

public:
    Algorithm(const TemplateTask& task);
    TemplateTask getTask();
    virtual std::vector<Points> getPoints() = 0;
    virtual Complexity getComplexity() = 0;
    virtual std::optional<TrialPoint> run() = 0;
};