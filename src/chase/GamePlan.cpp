#include "GamePlan.h"
#include "Chase.h"
#include "Util.h"

#include <evol/Rng.h>

namespace chase {

void GamePlan::crossover(GamePlan const& other)
{
    for(auto& [equity, percentage] : percentages)
    {
        if(auto it = other.percentages.find(equity); it != other.percentages.end())
        {
            percentage.gamble += it->second.gamble;
            percentage.gamble /= 2.0;
            percentage.stay += it->second.stay;
            percentage.stay /= 2.0;
        }
    }
}

void GamePlan::mutate()
{
    evol::Rng rng;
    size_t elementToMutate = rng.fetchUniform(0, percentages.size()-1, 1).top();
    auto it = percentages.begin();
    std::advance(it, elementToMutate);
    bool modifyGamble = rng.fetchUniform(0,1,1).top() == 0;
    int modifyPercentage = rng.fetchUniform(-10,10,1).top();
    if(modifyGamble)
        it->second.gamble += modifyPercentage / 100.0;
    else
        it->second.stay += modifyPercentage / 100.0;
    it->second.normalize();
}

std::string GamePlan::toString() const
{
    std::string ret = "\n";
    for( const auto& [index, percentage] : percentages)
        ret += "candidate " + std::to_string(index+1) + "(" + ( chase ? std::to_string(chase->candidates()[index].equity()*100) : "") + "%): " + percentage.toString() + "\n"; 
    return ret;
}

size_t GamePlan::Percentages::startingStep(int num) const
{
    if(eventHappens(gamble, num))
        return 6;
    else if(eventHappens(gamble+stay, num))
        return 5;
    else
        return 4;
}

void GamePlan::Percentages::normalize()
{
    if(gamble < 0) gamble = 0;
    if(stay < 0) stay = 0;
    if(gamble + stay > 1)
    {
        double div = gamble + stay;
        gamble *= 1.0 / div;
        stay *= 1.0 / div; 
    }
}

std::string GamePlan::Percentages::toString() const
{
    std::string ret;
    ret += "gamble(" + std::to_string(gamble*100) + "%), ";
    ret += "stay(" + std::to_string(stay*100) + "%), ";
    ret += "safety(" + std::to_string((1-gamble-stay)*100) + "%)";
    return ret;
}

}