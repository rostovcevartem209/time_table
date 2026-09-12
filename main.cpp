#include <iostream>
#include <windows.h>
#include <vector>
#include "data_manager.h"
#include "validator.h"
#include "genetic.h"
#include "fitness.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    DataManager dm;
    dm.loadTestData();

    std::cout << "Система автоматической генерации расписания\n";
    std::cout << "Разработчики Роман Хориков студент 1 курса Ростовцев Артём Кулик Макар Фишман Илья\n";
    std::cout << "--------------------------------------------------\n";

    int populationSize = 50;
    std::vector<std::vector<Gene>> population(populationSize);

    for (int i = 0; i < populationSize; ++i) {
        population[i] = GeneticAlgorithm::generateRandomSchedule(dm);
    }

    std::vector<Gene> bestSchedule = population[0];
    int bestPenalty = Fitness::calculatePenalty(bestSchedule);

    int generations = 100;
    for (int gen = 0; gen < generations; ++gen) {

        for (int i = 0; i < populationSize; ++i) {
            int penalty = Fitness::calculatePenalty(population[i]);
            if (penalty < bestPenalty) {
                bestPenalty = penalty;
                bestSchedule = population[i];
            }
        }

        if (bestPenalty == 0) {
            break;
        }

        std::vector<std::vector<Gene>> newPopulation(populationSize);
        for (int i = 0; i < populationSize; ++i) {
            // используем честный турнирный отбор родителей
            std::vector<Gene> parent1 = GeneticAlgorithm::tournamentSelection(population, dm);
            std::vector<Gene> parent2 = GeneticAlgorithm::tournamentSelection(population, dm);

            std::vector<Gene> child = GeneticAlgorithm::crossover(parent1, parent2);
            GeneticAlgorithm::mutate(child, dm);

            newPopulation[i] = child;
        }

        population = newPopulation;
    }

    std::cout << "Эволюция с турнирным отбором завершена\n";
    std::cout << "Лучший штрафной балл " << bestPenalty << "\n";
    std::cout << "Жестких конфликтов " << Validator::countHardConflicts(bestSchedule) << "\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "Итоговое сгенерированное расписание:\n";

    // выводим расшифровку расписания
    for (const Gene& gene : bestSchedule) {
        std::cout << "Группа ID " << gene.groupId
            << " | Предмет ID " << gene.disciplineId
            << " | Слот времени " << gene.timeSlotId
            << " | Аудитория ID " << gene.roomId << "\n";
    }

    return 0;
}