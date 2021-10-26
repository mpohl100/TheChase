#include "TeamChase.h"

namespace chase {

TeamChase::TeamChase(std::vector<double> const& candidateChances, size_t numRounds, double chaserFactor, std::vector<int> const& path)
{
    size_t i = 0;
    for(const auto& candidateChance : candidateChances)
        candidates_.emplace_back(candidateChance, i++);
    chaser_ = Player(0.8, 0);
    numRounds_ = numRounds;
    chaserFactor_ = chaserFactor;
    dontPlayFinal_ = false;
    path_ = path;
}

void TeamChase::createGameState(evol::Rng const& rng) const
{
    if(gameState_)
        return;
    GameState gameState;
    for(size_t i = 0; i < path_.size(); ++i)
    {
        if( path_[i] == 0)
            continue;
        double playerAmount = playQuickRound(candidates_[i], rng);
        double factor = 0.0;
        switch(path_[i]){
            case 4: factor = 1.0 / chaserFactor_; break;
            case 5: factor = 1.0; break;
            case 6: factor = chaserFactor_; break;
            default: break;
        }
        gameState.result += playerAmount * factor;
        gameState.finalPlayers.push_back(candidates_[i]);
    }
    gameState_ = gameState;
}

double TeamChase::score(TeamGamePlan const& plan) const
{
    double ret = 0;
    evol::Rng rng;
    if(not gameState_)
        createGameState(rng);
    const GamePlan* gamePlan;
    if( auto it = plan.plan.find(path_); it != plan.plan.end())
        gamePlan = &it->second;
    else
        throw std::runtime_error("game plan of path not found in TeamGamePlan.");
    plan.currentPath = path_;
    size_t numWon = 0;
    for(size_t i = 0; i < numRounds_; ++i)
    {
        if(double result = play(plan, rng))
        {
            numWon++;
            ret += play(plan, rng);
        }
    }
    gamePlan->data.amountGained /= double(numRounds_);
    gamePlan->data.amountAvg = ret / double(numRounds_);
    gamePlan->data.percentageWon = double(numWon) / double(numRounds_);
    return gamePlan->data.amountAvg;
}

double TeamChase::play(TeamGamePlan const& gamePlan, evol::Rng const& rng) const
{
    // copy is a must
    GameState gameState = *gameState_;
    std::vector<int> path = path_;
    numGames_++;
    const GamePlan* currentPlan;
    if( auto it = gamePlan.plan.find(path); it != gamePlan.plan.end())
        currentPlan = &it->second;
    else
        throw std::runtime_error("game plan of path not found in TeamGamePlan.");
    currentPlan->data.amountInitial = gameState.result;
    for(size_t i = path_.size(); i < candidates_.size(); ++i)
    {
        const Player& candidate = candidates_[i];
        double playerAmount = playQuickRound(candidate, rng);
        const GamePlan* plan;
        if( auto it = gamePlan.plan.find(path); it != gamePlan.plan.end())
            plan = &it->second;
        else
            throw std::runtime_error("game plan of path not found in TeamGamePlan.");
        size_t step = playEscapeRound(candidate, *plan, rng, playerAmount);
        path.push_back(step);
        if(step > 0) 
        {
            if(i == path_.size())
                currentPlan->data.amountGained += playerAmount;
            gameState.result += playerAmount;
            gameState.finalPlayers.push_back(candidate);
        }
    }
    return playFinalRound(gameState, rng);
}



}