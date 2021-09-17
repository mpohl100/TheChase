#include "Rng.h"

#include <string>
#include <vector>

namespace chase{

void say_hello(std::string name);

class Player{
public:
    Player(double knows);
    
    Player() = default;
    Player(Player const&) = default;
    Player& operator=(Player const&) = default;
    Player(Player&&) = default;
    Player& operator=(Player&&) = default;

    bool answer(int num) const;
    bool answerPossibilities(int num) const;

private:
    double knows_ = 0.0;
};

struct GamePlan { // Chromosome class

};

class Chase { // Challenge class
public:
    Chase(double candidateChance, size_t numPlayers, size_t numRounds);
    Chase() = default;
    Chase(Chase const&) = default;
    Chase& operator=(Chase const&) = default;
    Chase(Chase&&) = default;
    Chase& operator=(Chase&&) = default;

    double score(GamePlan const& gamePlan);

private:
    double playQuickRound(Player const& player, evol::Rng const& rng);
    bool playEscapeRound(Player const& player, GamePlan const& gamePlan, evol::Rng const& rng);
    size_t deduceStartingStep(GamePlan const& gamePlan, evol::Rng const& rng);
    std::pair<int,int> playPlayersFinal(std::vector<std::reference_wrapper<const Player>> const& finalPlayers, evol::Rng const& rng, double expectedNumQuestions, double stdDev);
    double playFinalRound(double gainedAmount, std::vector<std::reference_wrapper<const Player>> const& finalPlayers, evol::Rng const& rng);
    double play(GamePlan const& gamePlan, evol::Rng const& rng);

    std::vector<Player> candidates_;
    Player chaser_;
    size_t numRounds_;
};

}