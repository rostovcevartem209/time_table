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

#include <fstream> 
#endif


// Сохраняем сырые ID расписания для Питона
void saveRawSchedule(const std::vector<Gene>& schedule) {
    std::ofstream out("schedule_raw.txt");
    for (const auto& g : schedule) {
        out << g.groupId << " " << g.disciplineId << " " << g.timeSlotId << " "
            << g.roomId << " " << g.teacherId << " " << g.isPinned << "\n";
    }
}

// Загружаем сырые ID обратно в память
std::vector<Gene> loadRawSchedule() {
    std::vector<Gene> schedule;
    std::ifstream in("schedule_raw.txt");
    Gene g;
    // Считываем данные в том же порядке
    while (in >> g.groupId >> g.disciplineId >> g.timeSlotId >> g.roomId >> g.teacherId >> g.isPinned) {
        schedule.push_back(g);
    }
    return schedule;
}

// функция для запуска эволюции
std::vector<Gene> runEvolution(const DataManager& dm, const std::vector<Gene>& existingSchedule = {}) {
    int populationSize = 50;
    std::vector<std::vector<Gene>> population(populationSize);

    for (int i = 0; i < populationSize; ++i) {
        std::vector<Gene> newIndividual = GeneticAlgorithm::generateRandomSchedule(dm);

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
    // передаем dm в расчет штрафа
    int bestPenalty = Fitness::calculatePenalty(bestSchedule, dm);

    int generations = 100;
    for (int gen = 0; gen < generations; ++gen) {
        for (int i = 0; i < populationSize; ++i) {
            // передаем dm в расчет штрафа
            int penalty = Fitness::calculatePenalty(population[i], dm);
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

    std::cout << "\nШпаргалка по времени\n";
    std::cout << "Понедельник слоты от 1 до 6\n";
    std::cout << "Вторник слоты от 7 до 12\n";
    std::cout << "Среда слоты от 13 до 18\n";
    std::cout << "Четверг слоты от 19 до 24\n";
    std::cout << "Пятница слоты от 25 до 30\n";

    int newTime;
    std::cout << "Введите новый слот времени от 1 до 30 ";
    std::cin >> newTime;

    std::cout << "\nСписок доступных аудиторий\n";
    const std::vector<Room>& rooms = dm.getRooms();
    for (const Room& r : rooms) {
        std::cout << "Айди " << r.id << " Название " << r.name << "\n";
    }

    int newRoom;
    std::cout << "Введите новый айди аудитории ";
    std::cin >> newRoom;

    int oldTime = schedule[index].timeSlotId;
    int oldRoom = schedule[index].roomId;

    schedule[index].timeSlotId = newTime;
    schedule[index].roomId = newRoom;

    // передаем dm в валидатор
    if (Validator::countHardConflicts(schedule, dm) > 0) {
        std::cout << "\nОшибка Перенос вызывает конфликт Изменения отменены\n";
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

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    DataManager dm;
    dm.loadFromFiles();

    // 1. Скрытый режим генерации
    if (argc > 1 && std::string(argv[1]) == "--auto") {
        std::vector<Gene> schedule = runEvolution(dm);
        Exporter::saveToFile("schedule.txt", schedule, dm);
        saveRawSchedule(schedule); // Сохраняем слепок памяти
        return 0;
    }

    // 2. Скрытый режим редактирования из интерфейса
        // Вызов: time_table.exe --edit <индекс> <новое_время> <новая_аудитория>
    if (argc > 4 && std::string(argv[1]) == "--edit") {
        int index = std::stoi(argv[2]);
        int newTime = std::stoi(argv[3]);
        int newRoom = std::stoi(argv[4]);

        std::vector<Gene> schedule = loadRawSchedule();
        if (index < 0 || index >= schedule.size()) return 1; // Ошибка индекса

        // Запоминаем, сколько конфликтов было ДО нашего переноса
        int conflictsBefore = Validator::countHardConflicts(schedule, dm);

        schedule[index].timeSlotId = newTime;
        schedule[index].roomId = newRoom;

        // Считаем конфликты ПОСЛЕ переноса
        int conflictsAfter = Validator::countHardConflicts(schedule, dm);

        // Блокируем, только если мы СОЗДАЛИ новую проблему
        if (conflictsAfter > conflictsBefore) {
            return 2; // Код 2 (Конфликт!)
        }

        // Если стало лучше или так же - сохраняем и закрепляем
        schedule[index].isPinned = true;
        Exporter::saveToFile("schedule.txt", schedule, dm);
        saveRawSchedule(schedule);

        return 0; // Код 0 (Успех)
    }

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
            currentSchedule = runEvolution(dm, currentSchedule);
            std::cout << "Эволюция завершена успешно\n";
            // передаем dm в расчет штрафа и конфликтов
            std::cout << "Штрафных баллов " << Fitness::calculatePenalty(currentSchedule, dm) << "\n";
            std::cout << "Жестких конфликтов " << Validator::countHardConflicts(currentSchedule, dm) << "\n";
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