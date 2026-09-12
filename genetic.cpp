#include "genetic.h"
#include <random>

std::vector<Gene> GeneticAlgorithm::generateRandomSchedule(const DataManager& dm) {
    std::vector<Gene> schedule;

    // настраиваем генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());

    // получаем списки из менеджера данных
    const std::vector<LessonPlan>& plans = dm.getLessonPlans();
    const std::vector<Room>& rooms = dm.getRooms();
    const std::vector<Teacher>& teachers = dm.getTeachers();

    // если данных нет возвращаем пустой список
    if (rooms.empty() || teachers.empty()) {
        return schedule;
    }

    // проходим по каждому плану для каждой группы
    for (const LessonPlan& plan : plans) {

        // создаем нужное количество занятий
        for (int i = 0; i < plan.requiredClasses; ++i) {
            Gene gene;

            // заполняем данные о группе и предмете
            gene.groupId = plan.groupId;
            gene.disciplineId = plan.disciplineId;

            // пока берем первого преподавателя
            gene.teacherId = teachers[0].id;

            // выбираем случайную аудиторию
            std::uniform_int_distribution<> roomDist(0, rooms.size() - 1);
            gene.roomId = rooms[roomDist(gen)].id;

            // выбираем случайное время от 1 до 30
            std::uniform_int_distribution<> timeDist(1, 30);
            gene.timeSlotId = timeDist(gen);

            // занятие не закреплено
            gene.isPinned = false;

            // добавляем занятие в расписание
            schedule.push_back(gene);
        }
    }

    return schedule;
}

std::vector<Gene> GeneticAlgorithm::crossover(const std::vector<Gene>& parent1, const std::vector<Gene>& parent2) {
    std::vector<Gene> child;
    size_t size = parent1.size();

    // генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());

    // бросаем монетку 0 или 1
    std::uniform_int_distribution<> dist(0, 1);

    // проходим по всем занятиям
    for (size_t i = 0; i < size; ++i) {

        // случайно берем ген от первого или второго родителя
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
    // если пусто ничего не делаем
    if (schedule.empty()) {
        return;
    }

    // генератор чисел
    std::random_device rd;
    std::mt19937 gen(rd());

    // выбираем случайное занятие
    std::uniform_int_distribution<> geneDist(0, schedule.size() - 1);
    int index = geneDist(gen);

    // решаем что менять 0 это время 1 это аудитория
    std::uniform_int_distribution<> coin(0, 1);

    if (coin(gen) == 0) {
        // меняем время
        std::uniform_int_distribution<> timeDist(1, 30);
        schedule[index].timeSlotId = timeDist(gen);
    }
    else {
        // меняем аудиторию
        const std::vector<Room>& rooms = dm.getRooms();
        if (!rooms.empty()) {
            std::uniform_int_distribution<> roomDist(0, rooms.size() - 1);
            schedule[index].roomId = rooms[roomDist(gen)].id;
        }
    }
}