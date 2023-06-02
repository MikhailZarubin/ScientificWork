#pragma once

#include "base_algorithm.hpp"


Algorithm::Algorithm(const TemplateTask& task) :
    _task(task) {}

TemplateTask Algorithm::getTask() {
    return _task;
}
