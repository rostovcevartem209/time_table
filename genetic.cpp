#include "genetic.h"
#include "fitness.h"
#include <random>
#include <algorithm>
#include <unordered_map>

std::vector<Gene> GeneticAlgorithm::generateRandomSchedule(const DataManager& dm) {
    std::vector<Gene> schedule;

    std::random_device rd;
    std::mt19937 gen(rd());

    const std::vector<LessonPlan>& plans = dm.getLessonPlans();
    const std::vector<Room>& rooms = dm.getRooms();
    const std::vector<Teacher>& teachers = dm.getTeachers();

    if (rooms.empty() || teachers.empty()) {
        return schedule;
    }

    // Словари для отслеживания занятости (чтобы не создавать жесткие конфликты на старте)
    // Формат: ID -> массив из 32 элементов (для таймслотов с 1 по 30)
    std::unordered_map<int, std::vector<bool>> roomOccupied;
    std::unordered_map<int, std::vector<bool>> teacherOccupied;
    std::unordered_map<int, std::vector<bool>> groupOccupied;

    for (const LessonPlan& plan : plans) {
        for (int i = 0; i < plan.requiredClasses; ++i) {
            Gene gene;
            gene.groupId = plan.groupId;
            gene.disciplineId = plan.disciplineId;
            gene.isPinned = false;

            // 1. Выбираем преподавателя
            std::vector<int> validTeachers;
            for (const Teacher& t : teachers) {
                if (t.disciplineId == plan.disciplineId) {
                    validTeachers.push_back(t.id);
                }
            }

            if (!validTeachers.empty()) {
                std::uniform_int_distribution<> teacherDist(0, validTeachers.size() - 1);
                gene.teacherId = validTeachers[teacherDist(gen)];
            }
            else {
                gene.teacherId = teachers[0].id;
            }

            // Инициализируем трекеры, если их еще нет
            if (teacherOccupied.find(gene.teacherId) == teacherOccupied.end()) {
                teacherOccupied[gene.teacherId] = std::vector<bool>(32, false);
            }
            if (groupOccupied.find(gene.groupId) == groupOccupied.end()) {
                groupOccupied[gene.groupId] = std::vector<bool>(32, false);
            }

            // 2. УМНЫЙ ПОИСК СВОБОДНОГО МЕСТА
            bool placed = false;

            // Перемешиваем таймслоты (1-30), чтобы не забивать только понедельники
            std::vector<int> timeSlots(30);
            for (int t = 0; t < 30; ++t) timeSlots[t] = t + 1;
            std::shuffle(timeSlots.begin(), timeSlots.end(), gen);

            // Перемешиваем индексы аудиторий
            std::vector<int> roomIndices(rooms.size());
            for (size_t r = 0; r < rooms.size(); ++r) roomIndices[r] = r;

            for (int t : timeSlots) {
                // Если преподаватель или группа уже заняты в это время - пропускаем
                if (groupOccupied[gene.groupId][t] || teacherOccupied[gene.teacherId][t]) {
                    continue;
                }

                std::shuffle(roomIndices.begin(), roomIndices.end(), gen);

                // Ищем первую свободную аудиторию
                for (int rIdx : roomIndices) {
                    int rId = rooms[rIdx].id;

                    if (roomOccupied.find(rId) == roomOccupied.end()) {
                        roomOccupied[rId] = std::vector<bool>(32, false);
                    }

                    if (!roomOccupied[rId][t]) {
                        // Нашли идеальное место!
                        gene.timeSlotId = t;
                        gene.roomId = rId;

                        // Бронируем место
                        roomOccupied[rId][t] = true;
                        groupOccupied[gene.groupId][t] = true;
                        teacherOccupied[gene.teacherId][t] = true;

                        placed = true;
                        break;
                    }
                }
                if (placed) break;
            }

            // Если расписание перегружено и идеального места не нашлось, 
            // ставим наугад (штраф за это потом исправит генетика)
            if (!placed) {
                std::uniform_int_distribution<> roomDist(0, rooms.size() - 1);
                gene.roomId = rooms[roomDist(gen)].id;

                std::uniform_int_distribution<> timeDist(1, 30);
                gene.timeSlotId = timeDist(gen);
            }

            schedule.push_back(gene);
        }
    }

    return schedule;
}

std::vector<Gene> GeneticAlgorithm::crossover(const std::vector<Gene>& parent1, const std::vector<Gene>& parent2) {
    std::vector<Gene> child;
    size_t size = parent1.size();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 1);

    for (size_t i = 0; i < size; ++i) {
        if (dist(gen) == 0) {
            child.push_back(parent1[i]);
        }
        else {
            child.push_back(parent2[i]);
        }
    }

    return child;
}

void GeneticAlgorithm::mutate(std::vector<Gene>& schedule, const DataManager& dm) {
    if (schedule.empty()) {
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> geneDist(0, schedule.size() - 1);
    int index = geneDist(gen);

    if (schedule[index].isPinned == true) {
        return;
    }

    std::uniform_int_distribution<> coin(0, 1);

    if (coin(gen) == 0) {
        std::uniform_int_distribution<> timeDist(1, 30);
        schedule[index].timeSlotId = timeDist(gen);
    }
    else {
        const std::vector<Room>& rooms = dm.getRooms();
        if (!rooms.empty()) {
            std::uniform_int_distribution<> roomDist(0, rooms.size() - 1);
            schedule[index].roomId = rooms[roomDist(gen)].id;
        }
    }
}

std::vector<Gene> GeneticAlgorithm::tournamentSelection(const std::vector<std::vector<Gene>>& population, const DataManager& dm) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, population.size() - 1);

    int index1 = dist(gen);
    int index2 = dist(gen);

    // добавили dm для правильного подсчета штрафа
    int penalty1 = Fitness::calculatePenalty(population[index1], dm);
    int penalty2 = Fitness::calculatePenalty(population[index2], dm);

    if (penalty1 < penalty2) {
        return population[index1];
    }
    else {
        return population[index2];
    }
}