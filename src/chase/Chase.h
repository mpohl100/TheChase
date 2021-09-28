#pragma once

#include "GamePlan.h"
#include "Player.h"

#include <evol/Rng.h>

#include <vector>

namespace chase{

class Chase { // Challenge class
public:
    Chase(double candidateChance, size_t numPlayers, size_t numRounds, double chaserFactor, bool dontPlayFinal);
    Chase() = default;
    Chase(Chase const&) = default;
    Chase& operator=(Chase const&) = default;
    Chase(Chase&&) = default;
    Chase& operator=(Chase&&) = default;

    double score(GamePlan const& gamePlan);
    std::vector<Player> const& candidates() const;
    size_t numGames() const;
private:
    double playQuickRound(Player const& player, evol::Rng const& rng);
    bool playEscapeRound(Player const& player, GamePlan const& gamePlan, evol::Rng const& rng, double& amount);
    std::pair<int,int> playPlayersFinal(std::vector<std::reference_wrapper<const Player>> const& finalPlayers, evol::Rng const& rng, double expectedNumQuestions, double stdDev);
    double playFinalRound(double gainedAmount, std::vector<std::reference_wrapper<const Player>> const& finalPlayers, evol::Rng const& rng);
    double play(GamePlan const& gamePlan, evol::Rng const& rng);

    std::vector<Player> candidates_;
    Player chaser_;
    size_t numRounds_;
    double chaserFactor_ = 3.0;
    bool dontPlayFinal_ = false;
    size_t numGames_ = 0;
};

}