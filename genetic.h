#pragma once
#include "models.h"
#include "data_manager.h"
#include <vector>

class GeneticAlgorithm {
public:
    // создание случайного расписания
    static std::vector<Gene> generateRandomSchedule(const DataManager& dm);

    // скрещивание двух расписаний родителей
    static std::vector<Gene> crossover(const std::vector<Gene>& parent1, const std::vector<Gene>& parent2);

    // мутация случайное изменение одного занятия
    static void mutate(std::vector<Gene>& schedule, const DataManager& dm);
};