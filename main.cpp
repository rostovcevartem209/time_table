#include <iostream>
#include <windows.h>
#include <vector>
#include "data_manager.h"
#include "validator.h"
#include "genetic.h"
#include "fitness.h"
#include "exporter.h"

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
            std::vector<Gene> parent1 = GeneticAlgorithm::tournamentSelection(population, dm);
            std::vector<Gene> parent2 = GeneticAlgorithm::tournamentSelection(population, dm);

            std::vector<Gene> child = GeneticAlgorithm::crossover(parent1, parent2);
            GeneticAlgorithm::mutate(child, dm);

            newPopulation[i] = child;
        }

        population = newPopulation;
    }

    // сохраняем результат в файл
    Exporter::saveToFile("schedule.txt", bestSchedule, dm);

    std::cout << "Эволюция завершена успешно\n";
    std::cout << "Лучший штрафной балл " << bestPenalty << "\n";
    std::cout << "Жестких конфликтов " << Validator::countHardConflicts(bestSchedule) << "\n";
    std::cout << "Результат успешно сохранен в файл schedule.txt\n";

    return 0;
}