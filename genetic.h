#pragma once
#include "models.h"
#include "data_manager.h"
#include <vector>

class GeneticAlgorithm {
public:
    static std::vector<Gene> generateRandomSchedule(const DataManager& dm);
};
