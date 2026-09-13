#pragma once
#include "models.h"
#include <vector>
#include <string>

class DataManager {
private:
    std::vector<Group> groups;
    std::vector<Teacher> teachers;
    std::vector<Discipline> disciplines;
    std::vector<Room> rooms;
    std::vector<LessonPlan> lessonPlans;

public:
    void loadTestData();

    // новая функция для загрузки из файлов
    void loadFromFiles();

    const std::vector<Group>& getGroups() const;
    const std::vector<Teacher>& getTeachers() const;
    const std::vector<Discipline>& getDisciplines() const;
    const std::vector<Room>& getRooms() const;
    const std::vector<LessonPlan>& getLessonPlans() const;

    // функции для поиска названий
    std::string getGroupName(int id) const;
    std::string getTeacherName(int id) const;
    std::string getDisciplineName(int id) const;
    std::string getRoomName(int id) const;
    // функция для перевода слота в день недели и номер пары
    std::string formatTime(int timeSlotId) const;
};