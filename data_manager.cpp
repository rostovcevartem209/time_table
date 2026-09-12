#include "data_manager.h"

// --- Реализация методов добавления ---
void DataManager::addGroup(const Group& group) {
    groups.push_back(group);
}

void DataManager::addTeacher(const Teacher& teacher) {
    teachers.push_back(teacher);
}

void DataManager::addRoom(const Room& room) {
    rooms.push_back(room);
}

void DataManager::addDiscipline(const Discipline& discipline) {
    disciplines.push_back(discipline);
}

void DataManager::addLessonPlan(const LessonPlan& plan) {
    lessonPlans.push_back(plan);
}

// --- Реализация методов получения данных ---
const std::vector<Group>& DataManager::getGroups() const { return groups; }
const std::vector<Teacher>& DataManager::getTeachers() const { return teachers; }
const std::vector<Room>& DataManager::getRooms() const { return rooms; }
const std::vector<Discipline>& DataManager::getDisciplines() const { return disciplines; }
const std::vector<LessonPlan>& DataManager::getLessonPlans() const { return lessonPlans; }

// --- Загрузка тестового набора данных ---
void DataManager::loadTestData() {
    // 1. Добавляем учебные группы
    addGroup({ 1, "МИ-11", 25 });
    addGroup({ 2, "ИВТ-21", 30 });

    // 2. Добавляем дисциплины (ID, Название, Тип, Часов в неделю, Нужны ли ПК)
    addDiscipline({ 1, "Цифровая культура", LECTURE, 2, false });
    addDiscipline({ 2, "Программирование на C++", PRACTICE, 3, true });

    // 3. Добавляем преподавателей (ID, ФИО, ID доступных дисциплин, ID доступных слотов)
    addTeacher({ 1, "Нефёдова О.Г.", {1}, {1, 2, 3, 4, 5} });
    addTeacher({ 2, "Иванов И.И.", {2}, {1, 2, 3, 4} });

    // 4. Добавляем аудитории (ID, Название, Вместимость, Наличие ПК)
    addRoom({ 1, "Аудитория 305", 35, true });
    addRoom({ 2, "Аудитория 412", 50, false });

    // 5. Формируем учебный план (ID группы, ID дисциплины, Количество пар)
    addLessonPlan({ 1, 1, 2 }); // Группе МИ-11 нужно 2 пары Цифровой культуры
    addLessonPlan({ 2, 2, 3 }); // Группе ИВТ-21 нужно 3 пары Программирования
}