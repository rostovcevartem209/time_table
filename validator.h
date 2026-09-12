#pragma once
#include "models.h"
#include <vector>

class Validator {
public:
    // Функция принимает готовое расписание и возвращает количество конфликтов
    static int countHardConflicts(const std::vector<Gene>& schedule);
};
