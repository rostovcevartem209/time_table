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

    // создаем начальную популяцию из пятидесяти расписаний
    int populationSize = 50;
    std::vector<std::vector<Gene>> population(populationSize);

    for (int i = 0; i < populationSize; ++i) {
        population[i] = GeneticAlgorithm::generateRandomSchedule(dm);
    }

    // переменные для хранения лучшего результата за все поколения
    std::vector<Gene> bestSchedule = population[0];
    int bestPenalty = Fitness::calculatePenalty(bestSchedule);

    // запускаем цикл эволюции на сто поколений
    int generations = 100;
    for (int gen = 0; gen < generations; ++gen) {

        // проверяем каждое расписание в популяции
        for (int i = 0; i < populationSize; ++i) {
            int penalty = Fitness::calculatePenalty(population[i]);

            // если нашли вариант лучше сохраняем его
            if (penalty < bestPenalty) {
                bestPenalty = penalty;
                bestSchedule = population[i];
            }
        }

        // если нашли идеальное расписание без штрафов прерываем цикл
        if (bestPenalty == 0) {
            break;
        }

        // создаем новое поколение методом скрещивания и мутации
        std::vector<std::vector<Gene>> newPopulation(populationSize);
        for (int i = 0; i < populationSize; ++i) {
            std::vector<Gene> parent1 = population[0];
            std::vector<Gene> parent2 = population[1];

            std::vector<Gene> child = GeneticAlgorithm::crossover(parent1, parent2);
            GeneticAlgorithm::mutate(child, dm);

            newPopulation[i] = child;
        }

        population = newPopulation;
    }

    // выводим итоговые результаты работы генетического алгоритма
    std::cout << "Эволюция завершена успешно\n";
    std::cout << "Лучший штрафной балл " << bestPenalty << "\n";
    std::cout << "Количество жестких конфликтов " << Validator::countHardConflicts(bestSchedule) << "\n";

    return 0;
}