#pragma once
#include "models.h"
#include "data_manager.h"
#include <vector>
#include <string>

class Exporter {
public:
    // сохранение расписания в текстовый файл
    static void saveToFile(const std::string& filename, const std::vector<Gene>& schedule, const DataManager& dm);
};
