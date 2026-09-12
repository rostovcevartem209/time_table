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
    std::cout << "Разработчики студенты 2 курса Ростовцев Артём, Кулик Макар, Фишман Илья\n";
    std::cout << "--------------------------------------------------\n";

    // создаем переменные для хранения лучшего результата
    std::vector<Gene> bestSchedule;
    int minPenalty = 999999;

    // генерируем 1000 случайных вариантов
    for (int i = 1; i <= 1000; ++i) {

        // создаем случайное расписание
        std::vector<Gene> currentSchedule = GeneticAlgorithm::generateRandomSchedule(dm);

        // считаем штрафные баллы
        int currentPenalty = Fitness::calculatePenalty(currentSchedule);

        // если этот вариант лучше предыдущих сохраняем его
        if (currentPenalty < minPenalty) {
            minPenalty = currentPenalty;
            bestSchedule = currentSchedule;
        }
    }

    // выводим итог на экран
    std::cout << "Проверено 1000 вариантов расписания\n";
    std::cout << "Лучший результат имеет штраф " << minPenalty << " баллов\n";
    std::cout << "Количество жестких конфликтов " << Validator::countHardConflicts(bestSchedule) << "\n";

    return 0;
}