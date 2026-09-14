#include "validator.h"

int Validator::countHardConflicts(const std::vector<Gene>& schedule, const DataManager& dm) {
    int conflicts = 0;

    const std::vector<Group>& groups = dm.getGroups();
    const std::vector<Room>& rooms = dm.getRooms();
    const std::vector<Discipline>& disciplines = dm.getDisciplines();
    const std::vector<Teacher>& teachers = dm.getTeachers();

    for (size_t i = 0; i < schedule.size(); ++i) {

        // проверка вместимости и типа аудитории
        int students = 0;
        for (const Group& g : groups) {
            if (g.id == schedule[i].groupId) students = g.studentsCount;
        }

        int roomCapacity = 0;
        int roomType = 0;
        for (const Room& r : rooms) {
            if (r.id == schedule[i].roomId) {
                roomCapacity = r.capacity;
                roomType = r.type;
            }
        }

        if (students > roomCapacity) {
            conflicts++;
        }

        // проверка оборудования компьютерный класс для программирования
        int discType = 0;
        for (const Discipline& d : disciplines) {
            if (d.id == schedule[i].disciplineId) discType = d.type;
        }

        if (discType == 1 && roomType != 1) {
            conflicts++;
        }

        // проверка доступности преподавателя
        int unavailDay = -1;
        for (const Teacher& t : teachers) {
            if (t.id == schedule[i].teacherId) unavailDay = t.unavailableDay;
        }

        int currentDay = (schedule[i].timeSlotId - 1) / 6;
        if (currentDay == unavailDay) {
            conflicts++;
        }

        // проверка накладок расписания
        for (size_t j = i + 1; j < schedule.size(); ++j) {
            if (schedule[i].timeSlotId == schedule[j].timeSlotId) {
                if (schedule[i].groupId == schedule[j].groupId) conflicts++;
                if (schedule[i].teacherId == schedule[j].teacherId) conflicts++;
                if (schedule[i].roomId == schedule[j].roomId) conflicts++;
            }
        }
    }
    return conflicts;
}