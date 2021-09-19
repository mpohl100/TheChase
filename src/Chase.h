#pragma once

#include "Rng.h"

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace chase{

class Chase;

struct GamePlan { // Chromosome class
    
    void crossover(GamePlan const& other);
    void mutate();
    std::string toString() const;

    struct Percentages{
        double gamble = 0.2;
        double stay = 0.6;
        // double safety = 0.2 is calcualted implicitly
        size_t startingStep(int num) const;
        void normalize();
        std::string toString() const;
    };

    //       Player index
    std::map<size_t, Percentages> percentages;
    std::shared_ptr<Chase> chase;
};

class Player{
public:
    Player(double knows, size_t index);
    
    Player() = default;
    Player(Player const&) = default;
    Player& operator=(Player const&) = default;
    Player(Player&&) = default;
    Player& operator=(Player&&) = default;

    bool answer(int num) const;
    bool answerPossibilities(int num) const;
    size_t deduceStartingStep(GamePlan const& gamePlan, evol::Rng const& rng) const;
    double equity() const;
    size_t index() const;

private:
    double knows_ = 0.0;
    size_t index_ = 0;
};

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
};

}