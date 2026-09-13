#include <iostream>
#include <vector>
#include <string>
#include "data_manager.h"
#include "validator.h"
#include "genetic.h"
#include "fitness.h"
#include "exporter.h"

#ifdef _WIN32
#include <windows.h>
#endif

// функция для запуска эволюции
std::vector<Gene> runEvolution(const DataManager& dm, const std::vector<Gene>& existingSchedule = {}) {
    int populationSize = 50;
    std::vector<std::vector<Gene>> population(populationSize);

    // если есть старое расписание мы должны сохранить закрепленные пары
    for (int i = 0; i < populationSize; ++i) {
        std::vector<Gene> newIndividual = GeneticAlgorithm::generateRandomSchedule(dm);

        // переносим закрепленные гены из старого расписания
        if (!existingSchedule.empty()) {
            for (size_t j = 0; j < existingSchedule.size(); ++j) {
                if (existingSchedule[j].isPinned) {
                    newIndividual[j] = existingSchedule[j];
                }
            }
        }
        population[i] = newIndividual;
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

            // защищаем закрепленные гены от случайного изменения
            if (!existingSchedule.empty()) {
                for (size_t j = 0; j < existingSchedule.size(); ++j) {
                    if (existingSchedule[j].isPinned) {
                        child[j] = existingSchedule[j];
                    }
                }
            }

            newPopulation[i] = child;
        }
        population = newPopulation;
    }
    return bestSchedule;
}

// функция ручного редактирования
void editSchedule(std::vector<Gene>& schedule, const DataManager& dm) {
    if (schedule.empty()) {
        std::cout << "\nРасписание пустое Сначала сгенерируйте его\n";
        return;
    }

    std::cout << "\nСписок занятий для редактирования\n";
    for (size_t i = 0; i < schedule.size(); ++i) {
        std::cout << "Номер " << i << " Группа " << dm.getGroupName(schedule[i].groupId)
            << " Предмет " << dm.getDisciplineName(schedule[i].disciplineId)
            << " Время " << dm.formatTime(schedule[i].timeSlotId);

        if (schedule[i].isPinned) {
            std::cout << " ЗАКРЕПЛЕНО";
        }
        std::cout << "\n";
    }

    int index;
    std::cout << "Введите номер занятия для изменения или минус 1 для отмены ";
    std::cin >> index;

    if (index < 0 || index >= schedule.size()) {
        return;
    }

    int newTime, newRoom;
    std::cout << "Введите новый слот времени от 1 до 30 ";
    std::cin >> newTime;
    std::cout << "Введите новый айди аудитории ";
    std::cin >> newRoom;

    // сохраняем старые значения на случай ошибки
    int oldTime = schedule[index].timeSlotId;
    int oldRoom = schedule[index].roomId;

    // применяем изменения
    schedule[index].timeSlotId = newTime;
    schedule[index].roomId = newRoom;

    // проверяем не сломали ли мы расписание
    if (Validator::countHardConflicts(schedule) > 0) {
        std::cout << "\nОшибка Перенос вызывает конфликт Изменения отменены\n";
        // откатываем назад
        schedule[index].timeSlotId = oldTime;
        schedule[index].roomId = oldRoom;
    }
    else {
        std::cout << "\nИзменения успешно применены Конфликтов нет\n";

        int pinChoice = 0;
        std::cout << "Закрепить это занятие чтобы алгоритм его больше не трогал 1 да 0 нет ";
        std::cin >> pinChoice;

        if (pinChoice == 1) {
            schedule[index].isPinned = true;
            std::cout << "Занятие надежно закреплено\n";
        }
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    DataManager dm;
    dm.loadFromFiles();

    std::vector<Gene> currentSchedule;
    bool isRunning = true;
    int choice = 0;

    while (isRunning == true) {
        std::cout << "\n==================================================\n";
        std::cout << "СИСТЕМА АВТОМАТИЧЕСКОЙ ГЕНЕРАЦИИ РАСПИСАНИЯ\n";
        std::cout << "Разработчики Ростовцев Артём Кулик Макар Фишман Илья\n";
        std::cout << "==================================================\n";
        std::cout << "1 Сгенерировать расписание\n";
        std::cout << "2 Показать расписание на экране\n";
        std::cout << "3 Экспортировать в файл txt\n";
        std::cout << "4 Ручное редактирование\n";
        std::cout << "0 Выход\n";
        std::cout << "Ваш выбор ";

        std::cin >> choice;

        if (choice == 1) {
            std::cout << "\nЗапуск алгоритма подождите\n";
            // передаем текущее расписание чтобы сохранить закрепленные пары
            currentSchedule = runEvolution(dm, currentSchedule);
            std::cout << "Эволюция завершена успешно\n";
            std::cout << "Штрафных баллов " << Fitness::calculatePenalty(currentSchedule) << "\n";
            std::cout << "Жестких конфликтов " << Validator::countHardConflicts(currentSchedule) << "\n";
        }
        else if (choice == 2) {
            if (currentSchedule.empty()) {
                std::cout << "\nРасписание еще не сгенерировано\n";
            }
            else {
                std::cout << "\nТекущее расписание\n";
                for (const Gene& gene : currentSchedule) {
                    std::cout << "Группа " << dm.getGroupName(gene.groupId)
                        << " Предмет " << dm.getDisciplineName(gene.disciplineId)
                        << " Время " << dm.formatTime(gene.timeSlotId)
                        << " Аудитория " << dm.getRoomName(gene.roomId)
                        << " Преподаватель " << dm.getTeacherName(gene.teacherId);
                    if (gene.isPinned) std::cout << " ЗАКРЕПЛЕНО";
                    std::cout << "\n";
                }
            }
        }
        else if (choice == 3) {
            if (currentSchedule.empty()) {
                std::cout << "\nНет данных для экспорта\n";
            }
            else {
                Exporter::saveToFile("schedule.txt", currentSchedule, dm);
                std::cout << "\nРасписание сохранено\n";
            }
        }
        else if (choice == 4) {
            editSchedule(currentSchedule, dm);
        }
        else if (choice == 0) {
            std::cout << "\nЗавершение работы\n";
            isRunning = false;
        }
        else {
            std::cout << "\nНеверный ввод\n";
        }
    }

    return 0;
}