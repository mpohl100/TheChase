#pragma once

#include "Chase.h"
#include "TeamGamePlan.h"

namespace chase{

class TeamChase : public Chase{
public:
    TeamChase(std::vector<double> const& candidateChances, size_t numRounds, double chaserFactor, std::vector<int> const& path);
    double score(TeamGamePlan const& teamGamePlan) const;
    TeamChase() = default;
    TeamChase(TeamChase const&) = default;
    TeamChase& operator=(TeamChase const&) = default;
private:
    void createGameState(evol::Rng const& rng) const;
    double play(TeamGamePlan const& gamePlan, evol::Rng const& rng) const;

    std::vector<int> path_;
    mutable std::optional<GameState> gameState_;
};

}