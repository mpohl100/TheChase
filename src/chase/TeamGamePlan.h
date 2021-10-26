#pragma once

#include "GamePlan.h"

#include <map>
#include <vector>
#include <string>

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

template<class T>
std::string toStringPath(std::vector<T> const& path)
{
    if(path.empty())
        return "";
    std::string ret = "[";
    for(const auto& num : path)
        ret += std::to_string(num) + "|";
    ret = ret.substr(0, ret.size() - 1);
    ret += "]";
    return ret;
}

}