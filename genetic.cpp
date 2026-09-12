#include "genetic.h"
#include "fitness.h"
#include <random>

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

    for (const LessonPlan& plan : plans) {
        for (int i = 0; i < plan.requiredClasses; ++i) {
            Gene gene;
            gene.groupId = plan.groupId;
            gene.disciplineId = plan.disciplineId;
            gene.teacherId = teachers[0].id;

            std::uniform_int_distribution<> roomDist(0, rooms.size() - 1);
            gene.roomId = rooms[roomDist(gen)].id;

            std::uniform_int_distribution<> timeDist(1, 30);
            gene.timeSlotId = timeDist(gen);

            gene.isPinned = false;
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

    // выбираем двух случайных кандидатов из популяции
    int index1 = dist(gen);
    int index2 = dist(gen);

    int penalty1 = Fitness::calculatePenalty(population[index1]);
    int penalty2 = Fitness::calculatePenalty(population[index2]);

    // возвращаем того у кого штраф меньше
    if (penalty1 < penalty2) {
        return population[index1];
    }
    else {
        return population[index2];
    }
}