#pragma once
#include "models.h"
#include "data_manager.h"
#include <vector>

class Fitness {
public:
    static int calculatePenalty(const std::vector<Gene>& schedule, const DataManager& dm);
};