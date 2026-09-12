#pragma once
#include "models.h"
#include "validator.h"
#include <vector>

class Fitness {
public:
    static int calculatePenalty(const std::vector<Gene>& schedule);
};
