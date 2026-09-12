#include "validator.h"

int Validator::countHardConflicts(const std::vector<Gene>& schedule) {
    int conflicts = 0;
    size_t totalGenes = schedule.size();

    // Сравниваем каждую пару с каждой другой
    for (size_t i = 0; i < totalGenes; ++i) {
        for (size_t j = i + 1; j < totalGenes; ++j) {

            // Если две пары проходят в одно и то же время
            if (schedule[i].timeSlotId == schedule[j].timeSlotId) {

                // Проверка 1: Одна группа на двух занятиях
                if (schedule[i].groupId == schedule[j].groupId) {
                    conflicts++;
                }

                // Проверка 2: Один преподаватель ведет две пары
                if (schedule[i].teacherId == schedule[j].teacherId) {
                    conflicts++;
                }

                // Проверка 3: Две разные группы в одной аудитории
                if (schedule[i].roomId == schedule[j].roomId) {
                    conflicts++;
                }
            }
        }
    }

    return conflicts;
}