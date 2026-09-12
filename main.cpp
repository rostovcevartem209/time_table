#include <iostream>
#include <windows.h>
#include "data_manager.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    DataManager dm;
    dm.loadTestData();

    std::cout << "Система автоматической генерации расписания\n";
    std::cout << "Разработчик: Роман Хориков, студент 1 курса\n";
    std::cout << "--------------------------------------------------\n";

    std::cout << "Список учебных групп:\n";
    for (const auto& group : dm.getGroups()) {
        std::cout << "- " << group.name << ", количество студентов: " << group.studentsCount << "\n";
    }

    std::cout << "--------------------------------------------------\n";

    std::cout << "Список преподавателей:\n";
    for (const auto& teacher : dm.getTeachers()) {
        std::cout << "- " << teacher.name << "\n";
    }

    return 0;
}