#include "Chase.h"
#include "Rng.h"

#include <range/v3/all.hpp>

#include <iostream>
#include <cmath>
namespace chase{

void say_hello(std::string name)
{
    std::cout << "Hello, " << name << "!" << '\n';
}

Player::Player(double knows)
  : knows_(knows)
{}

bool Player::answer(int num) const
{
    return int(knows_ * 100) <= num;
}

bool Player::answerPossibilities(int num) const
{
    // we have a 1/3 chance of guessing correctly
    return int((knows_ + (1.0 - knows_) / 3) * 100) <= num;
}

Chase::Chase(double candidateChance, size_t numPlayers, size_t numRounds)
    : numRounds_(numRounds)
{
    for(size_t i = 0; i < numPlayers; ++i)
        candidates_.emplace_back(candidateChance);
    chaser_ = Player(0.8);
}


double Chase::score(GamePlan const& gamePlan)
{
    double ret = 0;
    evol::Rng rng;
    for(size_t i = 0; i < numRounds_; ++i)
        ret += play(gamePlan, rng);
    return ret / double(numRounds_);
}

double Chase::play(GamePlan const& gamePlan, evol::Rng const& rng)
{
    double result = 0;
    std::vector<std::reference_wrapper<const Player>> finalPlayers;
    for(const Player& candidate : candidates_)
    {
        double playerAmount = playQuickRound(candidate, rng);
        if(playEscapeRound(candidate, gamePlan, rng))
        {
            result += playerAmount;
            finalPlayers.push_back(candidate);
        }
    }
    return playFinalRound(result, finalPlayers, rng);
}

double Chase::playQuickRound(Player const& player, evol::Rng const& rng)
{
    size_t numQuestions = size_t(int(std::round(rng.fetchNormal(10.5, 0.5, 1).top())));
    std::stack randomProbabilites = rng.fetchUniform(0, 100, numQuestions);
    size_t numAnswered = 0;
    while(not randomProbabilites.empty())
        if(player.answer(randomProbabilites.top()))
            ++numAnswered;
    return numAnswered * 500.0; // 500€
}

bool Chase::playEscapeRound(Player const& candidate, GamePlan const& gamePlan, evol::Rng const& rng)
{
    size_t stepChaser = 8;
    size_t stepCandidate = deduceStartingStep(gamePlan, rng);
    std::stack<int> randomProbabilites = rng.fetchUniform(0, 100, 25); // 25 Zufallszahlen sollten erstmal garantiert reichen
    while(stepChaser > 0 or stepCandidate > 0 or stepChaser > stepCandidate)
    {
        // play candidate question
        if(randomProbabilites.empty())
            randomProbabilites = rng.fetchUniform(0, 100, 25);
        int randomProb = randomProbabilites.top();
        if(candidate.answerPossibilities(randomProb))
            stepCandidate--;

        // play chaser question
        if(randomProbabilites.empty())
            randomProbabilites = rng.fetchUniform(0, 100, 25);
        randomProb = randomProbabilites.top();
        if(chaser_.answerPossibilities(randomProb))
            stepChaser--;
    }
    return stepCandidate == 0 and stepChaser > 0;
}

size_t deduceStartingStep([[maybe_unused]] GamePlan const& gamePlan, [[maybe_unused]] evol::Rng const& rng)
{
    return 5;
}

std::pair<int, int> Chase::playPlayersFinal(std::vector<std::reference_wrapper<const Player>> const& finalPlayers, evol::Rng const& rng, double expectedNumQuestions, double stdDev)
{
    int playerScore = 0;
    size_t numPlayerQuestions = rng.fetchNormal(expectedNumQuestions, stdDev, 1).top();
    std::stack<int> randomProbabilites = rng.fetchUniform(0, 100 , numPlayerQuestions*finalPlayers.size());
    for(size_t i = 0; i < numPlayerQuestions; ++i)
    {
        size_t count = std::count_if(finalPlayers.begin(), finalPlayers.end(), [&randomProbabilites](const auto& player){
            return player.get().answer(randomProbabilites.top());
        });
        if(count > 0)
            playerScore++;
    }
    return { playerScore, numPlayerQuestions };
}

double Chase::playFinalRound(double gainedAmount, std::vector<std::reference_wrapper<const Player>> const& finalPlayers, evol::Rng const& rng)
{
    // play candidates final
    int playerScore = finalPlayers.size(); // candidates start with the amount of candidates as points
    playerScore += playPlayersFinal(finalPlayers, rng, 20.0, 2.0).first;
    // play chaser final
    std::vector<std::reference_wrapper<const Player>> chaserCont = { chaser_ };
    auto[chaserScore, chaserNumQuestions] = playPlayersFinal(chaserCont, rng, 22.0, 2.0); // chasers answers more quickly, therefore more questions

    chaserScore -= playPlayersFinal(finalPlayers, rng, double(chaserNumQuestions-chaserScore), 0.0).first;
    return playerScore > chaserScore ? gainedAmount : 0.0;
}

}