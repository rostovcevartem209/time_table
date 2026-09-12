#pragma once
#include <iostream>
#include <vector>
#include <string>

enum LessonType {
    LECTURE,
    PRACTICE,
    LAB
};

struct Group {
    int id;
    std::string name;
    int studentsCount;
};

struct Room {
    int id;
    std::string name;
    int capacity;
    bool hasComputers;
};

struct Teacher {
    int id;
    std::string name;
    std::vector<int> allowedDisciplines;
    std::vector<int> availableTimeSlots;
};

struct Discipline {
    int id;
    std::string name;
    LessonType type;
    int classesPerWeek;
    bool requiresComputers;
};

struct LessonPlan {
    int groupId;
    int disciplineId;
    int requiredClasses;
};

struct Gene {
    int groupId;
    int disciplineId;
    int teacherId;
    int roomId;
    int timeSlotId;
    bool isPinned;
};
