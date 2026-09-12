#include "fitness.h"

int Fitness::calculatePenalty(const std::vector<Gene>& schedule) {
    int penalty = 0;

    // считаем жесткие конфликты
    int conflicts = Validator::countHardConflicts(schedule);

    // за каждый конфликт даем 1000 штрафных баллов
    penalty = penalty + conflicts * 1000;

    // проходим по всем занятиям чтобы проверить мягкие ограничения
    for (size_t i = 0; i < schedule.size(); ++i) {

        // допустим что в дне 6 пар
        // проверяем является ли пара 5 или 6 по счету
        int pairNumber = schedule[i].timeSlotId % 6;

        // если остаток 5 или 0 значит пара поздняя
        if (pairNumber == 5 || pairNumber == 0) {
            // даем 10 штрафных баллов за вечернюю пару
            penalty = penalty + 10;
        }
    }

    return penalty;
}