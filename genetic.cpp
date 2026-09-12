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
