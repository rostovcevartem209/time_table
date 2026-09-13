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

    // записываем каждое занятие используя текстовые названия
    for (const Gene& gene : schedule) {
        file << "Группа " << dm.getGroupName(gene.groupId)
            << " | Предмет " << dm.getDisciplineName(gene.disciplineId)
            << " | Время " << gene.timeSlotId
            << " | Аудитория " << dm.getRoomName(gene.roomId)
            << " | Преподаватель " << dm.getTeacherName(gene.teacherId) << "\n";
    }

    // закрываем файл
    file.close();
}