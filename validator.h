#pragma once
#include "models.h"
#include "data_manager.h"
#include <vector>

class Validator {
public:
    static int countHardConflicts(const std::vector<Gene>& schedule, const DataManager& dm);
};