#pragma once

#include "GamePlan.h"

#include <map>
#include <vector>

namespace chase{

struct TeamGamePlan{
    std::map<std::vector<int>, GamePlan> plan;
    mutable std::vector<int> currentPath;
    void crossover(TeamGamePlan const& other);
    void mutate();
    std::string toString() const;
};

std::string toStringPath(std::vector<int> const& path);

}