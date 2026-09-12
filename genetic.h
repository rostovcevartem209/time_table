#pragma once
#include "models.h"
#include "data_manager.h"
#include <vector>

class GeneticAlgorithm {
public:
    static std::vector<Gene> generateRandomSchedule(const DataManager& dm);
    static std::vector<Gene> crossover(const std::vector<Gene>& parent1, const std::vector<Gene>& parent2);
};