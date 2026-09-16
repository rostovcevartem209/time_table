#include "fitness.h"
#include "validator.h"
#include <vector>

int Fitness::calculatePenalty(const std::vector<Gene>& schedule, const DataManager& dm) {
    int penalty = 0;

    int hardConflicts = Validator::countHardConflicts(schedule, dm);
    penalty += hardConflicts * 1000;

    int maxGroupId = 0;
    for (const auto& g : dm.getGroups()) {
        if (g.id > maxGroupId) maxGroupId = g.id;
    }

    // Структура для мгновенного подсчета окон без ресурсоемкой сортировки
    struct DayStats {
        int first = 99;
        int last = -1;
        int count = 0;
    };
    std::vector<std::vector<DayStats>> groupStats(maxGroupId + 1, std::vector<DayStats>(5));

    for (const Gene& gene : schedule) {
        int dayIndex = (gene.timeSlotId - 1) / 6;
        auto& stats = groupStats[gene.groupId][dayIndex];

        if (gene.timeSlotId < stats.first) stats.first = gene.timeSlotId;
        if (gene.timeSlotId > stats.last) stats.last = gene.timeSlotId;
        stats.count++;
    }

    // Оценка окон и перегрузов
    for (int g = 0; g <= maxGroupId; ++g) {
        for (int d = 0; d < 5; ++d) {
            const auto& stats = groupStats[g][d];
            if (stats.count > 1) {
                int expectedCount = stats.last - stats.first + 1;
                int windows = expectedCount - stats.count;
                if (windows > 0) {
                    penalty += windows * 10;
                }
            }
            if (stats.count > 4) {
                penalty += (stats.count - 4) * 20;
            }
        }
    }
    return penalty;
}