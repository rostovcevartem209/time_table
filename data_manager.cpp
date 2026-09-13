#include "data_manager.h"
#include <fstream>

void DataManager::loadTestData() {
    // оставляем старую функцию пустой чтобы не было ошибок сборки
}

void DataManager::loadFromFiles() {
    groups.clear();
    teachers.clear();
    disciplines.clear();
    rooms.clear();
    lessonPlans.clear();

    // загрузка групп
    std::ifstream fg("groups.txt");
    if (fg.is_open()) {
        int id;
        std::string name;
        while (fg >> id >> name) {
            Group g;
            g.id = id;
            g.name = name;
            g.studentsCount = 25;
            groups.push_back(g);
        }
        fg.close();
    }

    // загрузка преподавателей
    std::ifstream ft("teachers.txt");
    if (ft.is_open()) {
        int id;
        std::string name;
        while (ft >> id >> name) {
            Teacher t;
            t.id = id;
            t.name = name;
            teachers.push_back(t);
        }
        ft.close();
    }

    // загрузка дисциплин
    std::ifstream fd("disciplines.txt");
    if (fd.is_open()) {
        int id;
        std::string name;
        while (fd >> id >> name) {
            Discipline d;
            d.id = id;
            d.name = name;
            disciplines.push_back(d);
        }
        fd.close();
    }

    // загрузка аудиторий
    std::ifstream fr("rooms.txt");
    if (fr.is_open()) {
        int id;
        std::string name;
        while (fr >> id >> name) {
            Room r;
            r.id = id;
            r.name = name;
            r.capacity = 30;
            rooms.push_back(r);
        }
        fr.close();
    }

    // загрузка учебного плана
    // формат айдиГруппы айдиДисциплины количествоЗанятий
    std::ifstream fp("plans.txt");
    if (fp.is_open()) {
        int gId, dId, count;
        while (fp >> gId >> dId >> count) {
            LessonPlan lp;
            lp.groupId = gId;
            lp.disciplineId = dId;
            lp.requiredClasses = count;
            lessonPlans.push_back(lp);
        }
        fp.close();
    }
}

std::string DataManager::getGroupName(int id) const {
    for (const Group& g : groups) {
        if (g.id == id) return g.name;
    }
    return "Неизвестно";
}

std::string DataManager::getTeacherName(int id) const {
    for (const Teacher& t : teachers) {
        if (t.id == id) return t.name;
    }
    return "Неизвестно";
}

std::string DataManager::getDisciplineName(int id) const {
    for (const Discipline& d : disciplines) {
        if (d.id == id) return d.name;
    }
    return "Неизвестно";
}

std::string DataManager::getRoomName(int id) const {
    for (const Room& r : rooms) {
        if (r.id == id) return r.name;
    }
    return "Неизвестно";
}

const std::vector<Group>& DataManager::getGroups() const { return groups; }
const std::vector<Teacher>& DataManager::getTeachers() const { return teachers; }
const std::vector<Discipline>& DataManager::getDisciplines() const { return disciplines; }
const std::vector<Room>& DataManager::getRooms() const { return rooms; }
const std::vector<LessonPlan>& DataManager::getLessonPlans() const { return lessonPlans; }