#include "validator.h"
#include <vector>

int Validator::countHardConflicts(const std::vector<Gene>& schedule, const DataManager& dm) {
    int conflicts = 0;

    const auto& groups = dm.getGroups();
    const auto& rooms = dm.getRooms();
    const auto& disciplines = dm.getDisciplines();
    const auto& teachers = dm.getTeachers();

    // Находим максимальные ID для выделения непрерывных массивов
    int maxGroupId = 0, maxRoomId = 0, maxTeacherId = 0, maxDiscId = 0;
    for (const auto& g : groups) if (g.id > maxGroupId) maxGroupId = g.id;
    for (const auto& r : rooms) if (r.id > maxRoomId) maxRoomId = r.id;
    for (const auto& t : teachers) if (t.id > maxTeacherId) maxTeacherId = t.id;
    for (const auto& d : disciplines) if (d.id > maxDiscId) maxDiscId = d.id;

    // Векторы работают в сотни раз быстрее словарей!
    std::vector<int> groupStudents(maxGroupId + 1, 0);
    for (const auto& g : groups) groupStudents[g.id] = g.studentsCount;

    std::vector<int> roomCapacity(maxRoomId + 1, 0);
    std::vector<int> roomType(maxRoomId + 1, 0);
    for (const auto& r : rooms) { roomCapacity[r.id] = r.capacity; roomType[r.id] = r.type; }

    std::vector<int> discType(maxDiscId + 1, 0);
    for (const auto& d : disciplines) discType[d.id] = d.type;

    std::vector<int> teacherUnavail(maxTeacherId + 1, -1);
    for (const auto& t : teachers) teacherUnavail[t.id] = t.unavailableDay;

    // Битовые маски для таймслотов
    std::vector<unsigned int> roomTimeMask(maxRoomId + 1, 0);
    std::vector<unsigned int> groupTimeMask(maxGroupId + 1, 0);
    std::vector<unsigned int> teacherTimeMask(maxTeacherId + 1, 0);

    for (const Gene& gene : schedule) {
        if (groupStudents[gene.groupId] > roomCapacity[gene.roomId]) conflicts++;
        if (discType[gene.disciplineId] == 1 && roomType[gene.roomId] != 1) conflicts++;

        int currentDay = (gene.timeSlotId - 1) / 6;
        if (currentDay == teacherUnavail[gene.teacherId]) conflicts++;

        unsigned int slotBit = 1 << gene.timeSlotId;

        // Побитовые операции в регистрах процессора
        if (roomTimeMask[gene.roomId] & slotBit) conflicts++;
        else roomTimeMask[gene.roomId] |= slotBit;

        if (groupTimeMask[gene.groupId] & slotBit) conflicts++;
        else groupTimeMask[gene.groupId] |= slotBit;

        if (teacherTimeMask[gene.teacherId] & slotBit) conflicts++;
        else teacherTimeMask[gene.teacherId] |= slotBit;
    }

    return conflicts;
}