#include <iostream>
#include <vector>
#include <string>
#include "data_manager.h"
#include "validator.h"
#include "genetic.h"
#include "fitness.h"
#include "exporter.h"

// подключаем библиотеку только если это виндовс
#ifdef _WIN32
#include <windows.h>
#endif

// функция для запуска эволюции
std::vector<Gene> runEvolution(const DataManager& dm) {
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
    return bestSchedule;
}

int main() {
    // настраиваем кодировку только для виндовс
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif

    DataManager dm;
    
    // загружаем реальные данные из текстовых файлов
    dm.loadFromFiles();
    
    std::vector<Gene> currentSchedule;
    bool isRunning = true;
    int choice = 0;

    while (isRunning == true) {
        std::cout << "\n==================================================\n";
        std::cout << "СИСТЕМА АВТОМАТИЧЕСКОЙ ГЕНЕРАЦИИ РАСПИСАНИЯ\n";
        std::cout << "Разработчики Ростовцев Артём Кулик Макар Фишман Илья\n";
        std::cout << "==================================================\n";
        std::cout << "1 Сгенерировать новое расписание\n";
        std::cout << "2 Показать текущее расписание на экране\n";
        std::cout << "3 Экспортировать расписание в файл txt\n";
        std::cout << "0 Выход из программы\n";
        std::cout << "Ваш выбор ";
        
        std::cin >> choice;

        if (choice == 1) {
            std::cout << "\nЗапуск генетического алгоритма подождите\n";
            currentSchedule = runEvolution(dm);
            std::cout << "Эволюция завершена успешно\n";
            std::cout << "Штрафных баллов " << Fitness::calculatePenalty(currentSchedule) << "\n";
            std::cout << "Жестких конфликтов " << Validator::countHardConflicts(currentSchedule) << "\n";
        } 
        else if (choice == 2) {
            if (currentSchedule.empty()) {
                std::cout << "\nРасписание еще не сгенерировано\n";
            } else {
                std::cout << "\nТекущее расписание\n";
                for (const Gene& gene : currentSchedule) {
                    std::cout << "Группа " << dm.getGroupName(gene.groupId)
                        << " Предмет " << dm.getDisciplineName(gene.disciplineId)
                        << " Время " << dm.formatTime(gene.timeSlotId)
                        << " Аудитория " << dm.getRoomName(gene.roomId)
                        << " Преподаватель " << dm.getTeacherName(gene.teacherId) << "\n";
                }
            }
        }
        else if (choice == 3) {
            if (currentSchedule.empty()) {
                std::cout << "\nНет данных для экспорта\n";
            } else {
                Exporter::saveToFile("schedule.txt", currentSchedule, dm);
                std::cout << "\nРасписание сохранено в файл schedule.txt\n";
            }
        }
        else if (choice == 0) {
            std::cout << "\nЗавершение работы программы\n";
            isRunning = false;
        }
        else {
            std::cout << "\nНеверный ввод попробуйте снова\n";
        }
    }

    return 0;
}