#include "GamePlan.h"
#include "Chase.h"
#include "Util.h"

#include <evol/Rng.h>

namespace chase {

void GamePlan::crossover(GamePlan const& other)
{

    percentage.gamble += other.percentage.gamble;
    percentage.gamble /= 2.0;
    percentage.stay += other.percentage.stay;
    percentage.stay /= 2.0;
}

void GamePlan::mutate()
{
    evol::Rng rng;
    bool modifyGamble = rng.fetchUniform(0,1,1).top() == 0;
    int modifyPercentage = rng.fetchUniform(-10,10,1).top();
    if(modifyGamble)
        percentage.gamble += modifyPercentage / 100.0;
    else
        percentage.stay += modifyPercentage / 100.0;
    percentage.normalize();
}

std::string GamePlan::toString() const
{
    std::string ret = "\n";
    ret += "candidate: " + percentage.toString() + "\n"; 
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

std::string GamePlan::Percentages::toStringShort() const
{
    return "[" + std::to_string(gamble*100) + "%|" + std::to_string(stay*100) + "%|" + std::to_string((1-gamble-stay)*100) + "%]";
}

std::string GamePlan::Data::toStringShort() const
{
    return "[" + std::to_string(amountInitial) +"€|" + std::to_string(amountGained) + "€|" + std::to_string(amountAvg)+ "€|" + std::to_string(percentageWon*100) + "%]";
}

}