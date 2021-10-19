#pragma once

#include "GamePlan.h"

#include <map>
#include <vector>

namespace chase{

struct TeamGamePlan{
    TeamGamePlan() = default;
    TeamGamePlan(TeamGamePlan const&) = default;
    TeamGamePlan& operator=(TeamGamePlan const&) = default;
    TeamGamePlan(TeamGamePlan&&) = default;
    TeamGamePlan& operator=(TeamGamePlan&&) = default;
    
    std::map<std::vector<int>, GamePlan> plan;
    mutable std::vector<int> currentPath;
    void crossover(TeamGamePlan const& other);
    void mutate();
    std::string toString() const;
};

std::string toStringPath(std::vector<int> const& path);

}