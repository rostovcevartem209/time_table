#pragma once // Защита от повторного включения файла
#include "models.h"
#include <vector>

class DataManager {
private:
    // Векторы для хранения всех данных расписания
    std::vector<Group> groups;
    std::vector<Teacher> teachers;
    std::vector<Room> rooms;
    std::vector<Discipline> disciplines;
    std::vector<LessonPlan> lessonPlans;

public:
    // Методы для добавления новых записей в систему
    void addGroup(const Group& group);
    void addTeacher(const Teacher& teacher);
    void addRoom(const Room& room);
    void addDiscipline(const Discipline& discipline);
    void addLessonPlan(const LessonPlan& plan);

    // Методы для получения данных (возвращают константные ссылки для экономии памяти)
    const std::vector<Group>& getGroups() const;
    const std::vector<Teacher>& getTeachers() const;
    const std::vector<Room>& getRooms() const;
    const std::vector<Discipline>& getDisciplines() const;
    const std::vector<LessonPlan>& getLessonPlans() const;

    // Метод для быстрой загрузки тестовых данных
    void loadTestData();
};
