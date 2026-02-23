#pragma once

#include "Highs.h"
#include <string>

struct GameLP {
    HighsModel model;
    double shift;
};

GameLP loadGameFromCSV(const std::string& filepath);