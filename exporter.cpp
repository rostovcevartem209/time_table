#include "exporter.h"
#include <fstream>

void Exporter::saveToFile(const std::string& filename, const std::vector<Gene>& schedule, const DataManager& dm) {
    // открываем файл для записи
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }

    // записываем заголовок
    file << "Итоговое расписание занятий\n";
    file << "--------------------------------------------------\n";

    // записываем каждое занятие
    for (const Gene& gene : schedule) {
        file << "Группа ID " << gene.groupId
            << " | Предмет ID " << gene.disciplineId
            << " | Слот времени " << gene.timeSlotId
            << " | Аудитория ID " << gene.roomId << "\n";
    }

    // закрываем файл
    file.close();
}