#pragma once
#include <string>

struct Group {
    int id;
    std::string name;
    int studentsCount;
};

struct Teacher {
    int id;
    std::string name;
};

struct Discipline {
    int id;
    std::string name;
};

struct Room {
    int id;
    std::string name;
    int capacity;
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