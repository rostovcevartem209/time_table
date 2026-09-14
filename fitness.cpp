#include "fitness.h"
#include "validator.h"
#include <map>
#include <algorithm>

int Fitness::calculatePenalty(const std::vector<Gene>& schedule, const DataManager& dm) {
    int penalty = 0;

    int hardConflicts = Validator::countHardConflicts(schedule, dm);
    penalty += hardConflicts * 1000;

    std::map<int, std::map<int, std::vector<int>>> groupDays;

    for (const Gene& gene : schedule) {
        int dayIndex = (gene.timeSlotId - 1) / 6;
        groupDays[gene.groupId][dayIndex].push_back(gene.timeSlotId);
    }

    for (auto& groupPair : groupDays) {
        for (auto& dayPair : groupPair.second) {
            std::vector<int>& slots = dayPair.second;

            if (slots.size() > 1) {
                std::sort(slots.begin(), slots.end());

                int firstSlot = slots.front();
                int lastSlot = slots.back();

                int expectedCount = lastSlot - firstSlot + 1;
                int actualCount = slots.size();
                int windows = expectedCount - actualCount;

                // штраф за окна
                if (windows > 0) {
                    penalty += windows * 10;
                }
            }

            // штраф за перегрузку больше 4 пар в день
            if (slots.size() > 4) {
                penalty += (slots.size() - 4) * 20;
            }
        }
    }
    return penalty;
}