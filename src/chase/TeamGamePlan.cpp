#include "TeamGamePlan.h"

#include <stdexcept>

namespace chase {

void TeamGamePlan::crossover(TeamGamePlan const& other)
{
    if(currentPath != other.currentPath)
        throw std::runtime_error("different paths in crossover function.");
    auto it = other.plan.find(currentPath);
    if(it == other.plan.end())
        throw std::runtime_error("game plan for path not found in other.");
    plan[currentPath].crossover(it->second);
}

void TeamGamePlan::mutate()
{
    plan[currentPath].mutate();
}

std::string TeamGamePlan::toString() const
{
    auto it = plan.find(currentPath);
    if(it == plan.end())
        throw std::runtime_error("current path not found in TeamGamePlan::toString");
    return it->second.toString();
}

}