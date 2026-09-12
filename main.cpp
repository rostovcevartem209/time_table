#include <iostream>
#include <windows.h>
#include <vector>
#include "data_manager.h"
#include "validator.h"
#include "genetic.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    DataManager dm;
    dm.loadTestData();

    std::cout << "Система автоматической генерации расписания\n";
    std::cout << "Разработчики Ростовцев Артём Кулик Макар Фишман Илья\n";
    std::cout << "--------------------------------------------------\n";

    // генерируем 10 случайных расписаний
    for (int i = 1; i <= 10; ++i) {

        // создаем случайный вариант
        std::vector<Gene> randomSchedule = GeneticAlgorithm::generateRandomSchedule(dm);

        // считаем ошибки
        int conflicts = Validator::countHardConflicts(randomSchedule);

        // выводим результат на экран
        std::cout << "Вариант " << i << " создано занятий " << randomSchedule.size()
            << " конфликтов найдено " << conflicts << "\n";
    }

    return 0;
}