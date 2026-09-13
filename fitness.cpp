#include "fitness.h"
#include "validator.h"
#include <map>
#include <algorithm>

int Fitness::calculatePenalty(const std::vector<Gene>& schedule) {
    int penalty = 0;

    // жесткие конфликты дают огромный штраф чтобы алгоритм сразу их отбрасывал
    int hardConflicts = Validator::countHardConflicts(schedule);
    penalty += hardConflicts * 1000;

    // словарь для группировки занятий ключ это айди группы значение это дни
    std::map<int, std::map<int, std::vector<int>>> groupDays;

    for (const Gene& gene : schedule) {
        // вычисляем индекс дня от 0 до 4
        int dayIndex = (gene.timeSlotId - 1) / 6;

        // сохраняем номер слота для конкретной группы в конкретный день
        groupDays[gene.groupId][dayIndex].push_back(gene.timeSlotId);
    }

    // проходим по всем группам и считаем окна
    for (auto& groupPair : groupDays) {
        for (auto& dayPair : groupPair.second) {
            std::vector<int>& slots = dayPair.second;

            // если в день больше одной пары проверяем на окна
            if (slots.size() > 1) {

                // сортируем пары по времени по возрастанию
                std::sort(slots.begin(), slots.end());

                int firstSlot = slots.front();
                int lastSlot = slots.back();

                // сколько пар должно быть в идеале без пустых окон
                int expectedCount = lastSlot - firstSlot + 1;

                // сколько пар стоит на самом деле
                int actualCount = slots.size();

                // разница это и есть количество окон
                int windows = expectedCount - actualCount;

                if (windows > 0) {
                    // за каждое окно накидываем штраф 10 баллов
                    penalty += windows * 10;
                }
            }
        }
    }

    return penalty;
}