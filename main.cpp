#include <iostream>
#include <windows.h>
#include <vector>
#include "data_manager.h"
#include "validator.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    DataManager dm;
    dm.loadTestData();

    std::cout << "Система автоматической генерации расписания\n";
    std::cout << "Разработчик Роман Хориков студент 1 курса\n";
    std::cout << "--------------------------------------------------\n";

    // Создаем тестовое расписание
    std::vector<Gene> testSchedule;

    // Занятие 1 группа 1 предмет 1 преподаватель 1 аудитория 1 время 1
    testSchedule.push_back(Gene{ 1, 1, 1, 1, 1, false });

    // Конфликт 1 группа 1 идет на другой предмет в время 1
    testSchedule.push_back(Gene{ 1, 2, 2, 2, 1, false });

    // Конфликт 2 преподаватель 1 ведет у группы 2 в время 1
    testSchedule.push_back(Gene{ 2, 1, 1, 2, 1, false });

    // Запускаем проверку
    int conflictsCount = Validator::countHardConflicts(testSchedule);

    std::cout << "Количество занятий в тестовом расписании " << testSchedule.size() << "\n";
    std::cout << "Найдено жестких конфликтов " << conflictsCount << "\n";

    return 0;
}